#include "voyager/core/tcp_client.h"
#include "voyager/core/connector.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/util/logging.h"
#include "voyager/util/stringprintf.h"

namespace voyager {

TcpClient::TcpClient(EventLoop* ev, 
                     const SockAddr& addr, 
                     const std::string& name)
    : name_(name),
      server_ipbuf_(addr.Ipbuf()),
      ev_(CHECK_NOTNULL(ev)),
      connector_ptr_(new Connector(ev, addr)),
      conn_id_(0),
      connect_(false),
      retry_(false) {
  connector_ptr_->SetNewConnectionCallback(
      std::bind(&TcpClient::NewConnection, this, std::placeholders::_1));
  VOYAGER_LOG(INFO) << "TcpClient::TcpClient [" << name_ << "] is running";
}

TcpClient::~TcpClient() {
  VOYAGER_LOG(INFO) << "TcpClient::~TcpClient [" << name_ << "] is down";
}

void TcpClient::Connect() {
  VOYAGER_LOG(INFO) << "TcpClient::Connect - connecting to " 
                    << server_ipbuf_;
  connect_ = true;
  connector_ptr_->Start();
}

void TcpClient::ReConnect() {
  VOYAGER_LOG(INFO) << "TcpClient::ReConnect -reconnecting to " 
                    << server_ipbuf_;
  connect_ = true;
  connector_ptr_->ReStart();
}

void TcpClient::Close() {
  connect_ = false;
  connector_ptr_->Stop();
  TcpConnectionPtr ptr =  weak_ptr_.lock();
  if (ptr.get() != NULL) {
    ptr->ShutDown();
  }
}

void TcpClient::NewConnection(int socketfd) {
  ev_->AssertInMyLoop();

  std::string conn_name = StringPrintf("%s-%s#%d", name_.c_str(), 
                          server_ipbuf_.c_str(), ++conn_id_);

  VOYAGER_LOG(INFO) << "TcpClient::NewConnection[" << conn_name << "]";
  TcpConnectionPtr ptr(new TcpConnection(conn_name, ev_, socketfd));
 
  weak_ptr_ = ptr;

  ptr->SetConnectionCallback(connection_cb_);
  ptr->SetCloseCallback(std::bind(&TcpClient::HandleClose, 
                                  this, std::placeholders::_1));
  ptr->SetMessageCallback(message_cb_);
  ptr->SetWriteCompleteCallback(writecomplete_cb_);

  ev_->RunInLoop(std::bind(&TcpConnection::StartWorking, ptr));
}

void TcpClient::HandleClose(const TcpConnectionPtr& ptr) {
  if (close_cb_) {
    close_cb_(ptr);
  }
  if (retry_ && connect_) {
    connector_ptr_->ReStart();
  }
}

}  // namespace voyager
