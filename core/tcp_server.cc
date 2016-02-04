#include "core/tcp_server.h"

#include <functional>
#include <assert.h>

#include "core/acceptor.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"
#include "core/tcp_server.h"
#include "util/logging.h"

using namespace std::placeholders;

namespace mirants {

TcpServer::TcpServer(EventLoop* eventloop, 
                     const SockAddr& addr, 
                     const std::string& name,
                     int backlog)
    : eventloop_(eventloop),
      servinfo_(addr.AddrInfo()),
      acceptor_ptr_(new Acceptor(eventloop_, servinfo_, backlog)),
      name_(name) {
  acceptor_ptr_->SetNewConnectionCallback(
      std::bind(&TcpServer::NewConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
  MIRANTS_LOG(TRACE) << "TcpServer::~TcpServer [" << name_ << "] destructing";
}

void TcpServer::Start() {
  if (sequence_num.GetNext() == 0) {
    assert(!acceptor_ptr_->IsListenning());
    eventloop_->RunInLoop(
        std::bind(&Acceptor::EnableListen, acceptor_ptr_.get()));
  }
}

void TcpServer::NewConnection(int sockfd, const struct sockaddr_storage& sa) {
  eventloop_->AssertThreadSafe();
  MIRANTS_LOG(INFO) << "TcpServer::NewConnection [" << name_ << "]";
  
  TcpConnectionPtr conn_ptr(new TcpConnection());
  conn_ptr->SetConnectionCallback(connection_cb_);
  conn_ptr->SetWriteCompleteCallback(writecomplete_cb_);
  conn_ptr->SetCloseCallback(
      std::bind(&TcpServer::CloseConnection, this, _1));
}

void TcpServer::CloseConnection(const TcpConnectionPtr& conn_ptr) {
  eventloop_->RunInLoop(
      std::bind(&TcpServer::CloseConnectionInLoop, this, conn_ptr));
}

void TcpServer::CloseConnectionInLoop(const TcpConnectionPtr& conn_ptr) {
  eventloop_->AssertThreadSafe();
  connection_map_.erase(conn_ptr->conn_id()); 
}

}  // namespace mirants
