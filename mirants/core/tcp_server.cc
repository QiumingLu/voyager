#include "mirants/core/tcp_server.h"

#include <assert.h>
#include "mirants/core/acceptor.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/eventloop_threadpool.h"
#include "mirants/core/socket_util.h"
#include "mirants/core/tcp_connection.h"
#include "mirants/util/logging.h"
#include "mirants/util/stringprintf.h"

using namespace std::placeholders;

namespace mirants {

TcpServer::TcpServer(EventLoop* eventloop, 
                     const SockAddr& addr,
                     const std::string& name,
                     int thread_size,
                     int backlog)
    : eventloop_(CHECK_NOTNULL(eventloop)),
      addr_(addr),
      acceptor_ptr_(new Acceptor(eventloop_, addr_.AddrInfo(), backlog)),
      name_(name),
      ev_pool_(new EventLoopThreadPool(eventloop_, name_, thread_size-1)),
      conn_id_(0) {
  acceptor_ptr_->SetNewConnectionCallback(
      std::bind(&TcpServer::NewConnection, this, _1, _2));
}

TcpServer::~TcpServer() {
  eventloop_->AssertThreadSafe();
  MIRANTS_LOG(TRACE) << "TcpServer::~TcpServer [" << name_ << "] destructing";
  
  for (std::map<std::string, TcpConnectionPtr>::iterator it = connection_map_.begin();
       it != connection_map_.end(); ++it) {
    it->second->GetLoop()->RunInLoop(
        std::bind(&TcpConnection::DeleteConnection, it->second));
    it->second.reset();
  }
  addr_.FreeAddrinfo();
}

void TcpServer::Start() {
  if (sequence_num_.GetNext() == 0) {
    ev_pool_->Start();
    assert(!acceptor_ptr_->IsListenning());
    eventloop_->RunInLoop(
        std::bind(&Acceptor::EnableListen, acceptor_ptr_.get()));
  }
}

void TcpServer::NewConnection(int fd, const struct sockaddr_storage& sa) {
  eventloop_->AssertThreadSafe();

  std::string conn_name = 
      StringPrintf("%s-%s#%d", name_.c_str(), addr_.IP().c_str(), ++conn_id_);
  char peer[64];
  sockets::SockAddrToIPPort(reinterpret_cast<const sockaddr*>(&sa),
                            peer, sizeof(peer));
  MIRANTS_LOG(INFO) << "TcpServer::NewConnection [" << name_ 
                    << "] - new connection [" << conn_name
                    << "] from " << peer;

  EventLoop* ev = ev_pool_->GetNext(); 
  TcpConnectionPtr conn_ptr(new TcpConnection(conn_name, ev, fd));
  connection_map_[conn_name] = conn_ptr;
  conn_ptr->SetConnectionCallback(connection_cb_);
  conn_ptr->SetWriteCompleteCallback(writecomplete_cb_);
  conn_ptr->SetMessageCallback(message_cb_);
  conn_ptr->SetCloseCallback(
      std::bind(&TcpServer::CloseConnection, this, _1));
  ev->RunInLoop(std::bind(&TcpConnection::EstablishConnection, conn_ptr));
}

void TcpServer::CloseConnection(const TcpConnectionPtr& conn_ptr) {
  eventloop_->RunInLoop(
      std::bind(&TcpServer::CloseConnectionInLoop, this, conn_ptr));
}

void TcpServer::CloseConnectionInLoop(const TcpConnectionPtr& conn_ptr) {
  eventloop_->AssertThreadSafe();
  MIRANTS_LOG(INFO) << "TcpServer::CloseConnectionInLoop [" << name_ 
                    << "] - connection " << conn_ptr->name();
  connection_map_.erase(conn_ptr->name());
  conn_ptr->GetLoop()->QueueInLoop(
      std::bind(&TcpConnection::DeleteConnection, conn_ptr));
}

}  // namespace mirants
