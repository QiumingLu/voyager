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
      ev_(CHECK_NOTNULL(ev)),
      connector_ptr_(new Connector(ev, addr)),
      conn_id_(0),
      connect_(false) {
  connector_ptr_->SetNewConnectionCallback(
      std::bind(&TcpClient::NewConnection, this, std::placeholders::_1));
  VOYAGER_LOG(INFO) << "TcpClient::TcpClient [" << name_ << "] is running ";
}

TcpClient::~TcpClient() {
  VOYAGER_LOG(INFO) << "TcpClient::~TcpClient [" << name_ << "] is down";
}

void TcpClient::Connect() {
  VOYAGER_LOG(INFO) << "TcpClient::Connect - connecting to "
                    << connector_ptr_->ServerAddr().Ipbuf();
  connect_ = true;
  connector_ptr_->Start();
}

void TcpClient::ReConnect() {
  VOYAGER_LOG(INFO) << "TcpClient::ReConnect -reconnecting to "
                    << connector_ptr_->ServerAddr().Ipbuf();
  connect_ = true;
  connector_ptr_->ReStart();
}

void TcpClient::QuitConnect() {
  connect_ = false;
  connector_ptr_->Stop();
}

void TcpClient::DisConnect() {
  connect_ = false;
  if (conn_ptr_.get()) {
    conn_ptr_->ShutDown();
  }
}

void TcpClient::NewConnection(int socketfd) {
  ev_->AssertThreadSafe();

  std::string conn_name = 
      StringPrintf("%s-%s#%d", 
                  name_.c_str(), 
                  connector_ptr_->ServerAddr().Ipbuf().c_str(), 
                  ++conn_id_);

  TcpConnectionPtr ptr(new TcpConnection(conn_name, ev_, socketfd));

  ptr->SetConnectionCallback(connection_cb_);
  ptr->SetCloseCallback(close_cb_);
  ptr->SetMessageCallback(message_cb_);
  ptr->SetWriteCompleteCallback(writecomplete_cb_);

  conn_ptr_ = ptr;
  ptr->EstablishConnection();
}

}  // namespace voyager
