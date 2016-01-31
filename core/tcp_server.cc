#include "core/tcp_server.h"

#include <functional>
#include <assert.h>

#include "core/acceptor.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"

namespace mirants {

TcpServer::TcpServer(const SockAddr& addr, int backlog) 
    : servinfo_(addr.AddInfo()),
      acceptor_ptr_(new Acceptor(servinfo_, backlog)) {
}

TcpServer::~TcpServer() {
  MIRANTS_LOG(TRACE) << "TcpServer::~TcpServer [" << name_ << "] destructing";
}

void TcpServer::Start() {
  assert(!acceptor_ptr_->IsListenning());
  eventloop_.RunInLoop(
      std::bind(&Acceptor::EnableListen, acceptor_ptr_->get()));
}

}  // namespace mirants
