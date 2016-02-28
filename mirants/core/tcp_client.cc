#include "mirants/core/tcp_client.h"
#include "mirants/core/connector.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"
#include "mirants/core/tcp_connection.h"
#include "mirants/util/logging.h"
#include "mirants/util/stringprintf.h"

namespace mirants {

TcpClient::TcpClient(const std::string& name,
                     EventLoop* ev, const SockAddr& addr)
    : name_(name),
      ev_(CHECK_NOTNULL(ev)),
      connector_ptr_(new Connector(ev, addr)),
      conn_id_(0),
      retry_(false),
      connect_(false) {
  connector_ptr_->SetNewConnectionCallback(
      std::bind(&TcpClient::NewConnection, this, std::placeholders::_1));
  MIRANTS_LOG(INFO) << "TcpClient::TcpClient [" << name_ << "] - " << this;
}

TcpClient::~TcpClient() {
  TcpConnectionPtr p;
  bool unique = false;
  {
    port::MutexLock l(&mu_);
    unique = conn_ptr_.unique();
    p = conn_ptr_;
  }
  if (p.get()) {
    assert(ev_ == p->GetLoop());
    if (unique) {
      conn_ptr_->ForceClose();
    }
  } else {
    connector_ptr_->Stop();
  }
  MIRANTS_LOG(INFO) << "TcpClient::~TcpClient [" << name_ << "] - " << this;
}

void TcpClient::Connect() {
  MIRANTS_LOG(INFO) << "TcpClient::TcpConnect - connecting to "
                    << connector_ptr_->ServerAddr().IP();
  connect_ = true;
  connector_ptr_->Start();
}

void TcpClient::DisConnect() {
  connect_ = false;
  {
    port::MutexLock l(&mu_);
    if (conn_ptr_) {
      conn_ptr_->ShutDown();
    }
  }
}

void TcpClient::Stop() {
  connect_ = false;
  connector_ptr_->Stop();
}

void TcpClient::NewConnection(int socketfd) {
  ev_->AssertThreadSafe();

  std::string conn_name = 
      StringPrintf("%s-%s#%d", 
                  name_.c_str(), 
                  connector_ptr_->ServerAddr().IP().c_str(), 
                  ++conn_id_);

  TcpConnectionPtr p(new TcpConnection(conn_name, ev_, socketfd));

  p->SetConnectionCallback(connection_cb_);
  p->SetMessageCallback(message_cb_);
  p->SetWriteCompleteCallback(writecomplete_cb_);
  p->SetCloseCallback(
      std::bind(&TcpClient::CloseConnection, this,std::placeholders:: _1));
  
  {
    port::MutexLock l(&mu_);
    conn_ptr_ = p;
  }

  p->EstablishConnection();
}

void TcpClient::CloseConnection(const TcpConnectionPtr& conn) {
  ev_->AssertThreadSafe();
  assert(ev_ == conn->GetLoop());
  {
    port::MutexLock l(&mu_);
    assert(conn_ptr_ == conn);
    conn_ptr_.reset();
  }
  ev_->QueueInLoop(std::bind(&TcpConnection::DeleteConnection, conn));
  if (retry_ && connect_) {
    connector_ptr_->ReStart();
  }
}

}  // namespace mirants
