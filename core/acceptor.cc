#include "core/acceptor.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "core/eventloop.h"
#include "core/socket_util.h"
#include "util/logging.h"

namespace mirants {

Acceptor::Acceptor(EventLoop* eventloop, 
                   const struct addrinfo* addr, 
                   int backlog, 
                   bool reuseport)
    : eventloop_(eventloop),
      tcpsocket_(sockets::CreateSocketAndSetNonBlock(addr->ai_family)),
      dispatch_(eventloop_, tcpsocket_.SocketFd()),     
      backlog_(backlog),
      idlefd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)),
      listenning_(false) {
  assert(idlefd_ >= 0);
  tcpsocket_.SetReuseAddr(true);
  tcpsocket_.SetReusePort(reuseport);
  tcpsocket_.BindAddress(addr->ai_addr, addr->ai_addrlen);
  dispatch_.SetReadCallback(std::bind(&Acceptor::AcceptHandler, this));
}

Acceptor::~Acceptor() {
  dispatch_.DisableAll();
  dispatch_.RemoveEvents();
  ::close(idlefd_);
}

void Acceptor::EnableListen() {
  eventloop_->AssertThreadSafe();
  listenning_ = true;
  tcpsocket_.Listen(backlog_);
  dispatch_.EnableRead();
}

void Acceptor::AcceptHandler() {
  eventloop_->AssertThreadSafe();
  struct sockaddr_storage sa;
  socklen_t salen = static_cast<socklen_t>(sizeof(sa));
  int connectfd = tcpsocket_.Accept(reinterpret_cast<struct sockaddr*>(&sa), 
                                    &salen);
  if (connectfd >= 0) {
    if (connfunc_) {
      connfunc_(connectfd, sa);
    } else {
      sockets::CloseFd(connectfd);
    }
  } else {
    if (errno == EMFILE) {
      ::close(idlefd_);
      idlefd_ = ::accept(tcpsocket_.SocketFd(), NULL, NULL);
      ::close(idlefd_);
      idlefd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}

}  // namespace mirants
