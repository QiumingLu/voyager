#include "voyager/core/tcp_client.h"
#include "voyager/core/tcp_connector.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/util/logging.h"
#include "voyager/util/stringprintf.h"

namespace voyager {

port::SequenceNumber TcpClient::conn_id_;

TcpClient::TcpClient(EventLoop* ev,
                     const SockAddr& addr,
                     const std::string& name)
    : name_(name),
      server_ipbuf_(addr.Ipbuf()),
      ev_(CHECK_NOTNULL(ev)),
      connector_ptr_(new TcpConnector(ev, addr)),
      connect_(false) {
  connector_ptr_->SetNewConnectionCallback(
      std::bind(&TcpClient::NewConnection, this, std::placeholders::_1));
  VOYAGER_LOG(INFO) << "TcpClient::TcpClient [" << name_ << "] is running";
}

TcpClient::~TcpClient() {
  VOYAGER_LOG(INFO) << "TcpClient::~TcpClient [" << name_ << "] is down";
}

void TcpClient::Connect() {
  // 表示已经经过连接建立和连接断开的过程
  bool expected = true;

  if (!weak_ptr_.lock()) {
    connect_.compare_exchange_weak(expected, false);
  }

  if (!connect_) {
    VOYAGER_LOG(INFO) << "TcpClient::Connect - connecting to "
                      << server_ipbuf_;
    connector_ptr_->Start();
    connect_ = true;
  }
}

void TcpClient::Close() {
  // （1) 连接未成功
  // （2）连接成功但未断开
  if (connect_) {
    connector_ptr_->Stop();
    TcpConnectionPtr ptr = weak_ptr_.lock();
    if (ptr) {
      ptr->ShutDown();
    }
    connect_ = false;
  }
}

void TcpClient::NewConnection(int socketfd) {
  ev_->AssertInMyLoop();
  char ipbuf[64];
  SockAddr::FormatLocal(socketfd, ipbuf, sizeof(ipbuf));
  std::string conn_name = StringPrintf("%s-%s#%d",
                                       server_ipbuf_.c_str(),
                                       ipbuf,
                                       conn_id_.GetNext());
  VOYAGER_LOG(INFO) << "TcpClient::NewConnection[" << conn_name << "]";

  TcpConnectionPtr ptr(new TcpConnection(conn_name, ev_, socketfd));
  ptr->SetConnectionCallback(connection_cb_);
  ptr->SetMessageCallback(message_cb_);
  ptr->SetWriteCompleteCallback(writecomplete_cb_);
  ptr->SetCloseCallback(close_cb_);
  ev_->RunInLoop([ptr]() {
      ptr->StartWorking();
  });
  weak_ptr_ = ptr;
}

}  // namespace voyager
