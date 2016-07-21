#include "voyager/core/acceptor.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "voyager/core/sockaddr.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/socket_util.h"
#include "voyager/util/logging.h"

namespace voyager {

Acceptor::Acceptor(EventLoop* eventloop, 
                   const SockAddr& addr,
                   int backlog, 
                   bool reuseport)
    : eventloop_(eventloop),
      tcpsocket_(sockets::CreateSocketAndSetNonBlock(addr.Family())),
      dispatch_(eventloop_, tcpsocket_.SocketFd()),     
      backlog_(backlog),
      idlefd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)),
      listenning_(false) {
  assert(idlefd_ >= 0);
  tcpsocket_.SetReuseAddr(true);
  tcpsocket_.SetReusePort(reuseport);
  tcpsocket_.BindAddress(addr.GetSockAddr(), 
                         sizeof(*(addr.GetSockAddr())));
  dispatch_.SetReadCallback(std::bind(&Acceptor::Accept, this));
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

void Acceptor::Accept() {
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

}  // namespace voyager
