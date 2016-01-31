#include "core/tcp_server.h"

#include <functional>
#include <assert.h>

#include "core/acceptor.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"
#include "util/logging.h"

namespace mirants {

TcpServer::TcpServer(EventLoop* eventloop, const SockAddr& addr, int backlog) 
    : eventloop_(eventloop),
      servinfo_(addr.AddrInfo()),
      acceptor_ptr_(new Acceptor(eventloop_, servinfo_, backlog)) {
}

TcpServer::~TcpServer() {
  MIRANTS_LOG(TRACE) << "TcpServer::~TcpServer [" << name_ << "] destructing";
}

void TcpServer::Start() {
  assert(!acceptor_ptr_->IsListenning());
  eventloop_->RunInLoop(
      std::bind(&Acceptor::EnableListen, acceptor_ptr_.get()));
}

}  // namespace mirants
