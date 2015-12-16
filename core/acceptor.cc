#include "core/acceptor.h"
#include "core/socket_util.h"

namespace mirants {

Acceptor::Acceptor(const struct addrinfo* addr, bool reuseport)
    : tcpsocket_(sockets::CreateSocketAndSetNonBlock(addr->ai_family),
      listenning_(false) {
  tcpsocket_.SetReuseAddr(true);
  tcpsocket_.SetReusePort(reuseport);
  tcpsocket_.BindAddress(addr->ai_addr, addr->ai_addrlen);
}

Acceptor::~Acceptor() { }

void Acceptor::EnableListen() {
  listenning_ = true;
  tcpsocket_.Listen(backlog_);
}

void Acceptor::AcceptTcpHandler() {
  struct sockaddr_storage sa;
  int connectfd = tcpsocket_.Accept(reinterpret_cast<struct sockaddr*>(&sa), 
                                    static_cast<socklen_t>(sizeof(sa)));
  if (connectfd >= 0) {
  } else {
    if (errno == EMFILE) {
    }
  }
}

}  // namespace mirants
