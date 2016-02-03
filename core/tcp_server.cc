#include "core/tcp_server.h"

#include <functional>
#include <assert.h>

#include "core/acceptor.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"
#include "util/logging.h"

namespace mirants {

TcpServer::TcpServer(EventLoop* eventloop, 
                     const SockAddr& addr, 
                     const std::string& name,
                     int backlog)
    : eventloop_(eventloop),
      servinfo_(addr.AddrInfo()),
      acceptor_ptr_(new Acceptor(eventloop_, servinfo_, backlog)),
      name_(name) {
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

}  // namespace mirants
