#include "core/acceptor.h"

#include <functional>

#include "core/eventloop.h"
#include "core/socket_util.h"
#include "util/logging.h"

namespace mirants {

Acceptor::Acceptor(EventLoop* eventloop, const struct addrinfo* addr, int backlog, bool reuseport)
    : eventloop_(eventloop),
      tcpsocket_(sockets::CreateSocketAndSetNonBlock(addr->ai_family)),
      backlog_(backlog),
      listenning_(false),
      dispatch_(eventloop_, tcpsocket_.SocketFd()) {
  tcpsocket_.SetReuseAddr(true);
  tcpsocket_.SetReusePort(reuseport);
  tcpsocket_.BindAddress(addr->ai_addr, addr->ai_addrlen);
  dispatch_.SetReadCallBack(std::bind(&Acceptor::AcceptHandler, this));
}

Acceptor::~Acceptor() { }

void Acceptor::EnableListen() {
  eventloop_->AssertThreadSafe();
  listenning_ = true;
  tcpsocket_.Listen(backlog_);
  dispatch_.EnableRead();
}

void Acceptor::AcceptHandler() {
  struct sockaddr_storage sa;
  socklen_t salen = static_cast<socklen_t>(sizeof(sa));
  int connectfd = tcpsocket_.Accept(reinterpret_cast<struct sockaddr*>(&sa), &salen);
  if (connectfd >= 0) {
  } else {
    if (errno == EMFILE) {
    }
  }
}

}  // namespace mirants
