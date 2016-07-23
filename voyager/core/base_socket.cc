#include "voyager/core/base_socket.h"
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "voyager/util/logging.h"

namespace voyager {

BaseSocket::BaseSocket(int domain, bool nonblocking)
    : fd_(::socket(domain, SOCK_STREAM, IPPROTO_TCP)),
      need_close_(true) {
  if (fd_ == -1) {
    VOYAGER_LOG(FATAL) << "socket: " << strerror(errno);
  }
  SetNonBlockAndCloseOnExec(nonblocking);
}

BaseSocket::BaseSocket(int socketfd) 
  : fd_(socketfd),
    need_close_(true) {
}

BaseSocket::~BaseSocket() {
  if (need_close_) {
    if (::close(fd_) == -1) {
      VOYAGER_LOG(ERROR) << "close: " << strerror(errno);
    }
  }
}

void BaseSocket::ShutDownWrite() const {
  if (::shutdown(fd_, SHUT_WR) == -1) {
    VOYAGER_LOG(ERROR) << "shutdown: " << strerror(errno);
  }
}

void BaseSocket::SetNonBlockAndCloseOnExec(bool on) const {
  int flags = ::fcntl(fd_, F_GETFL, 0);
  if (flags == -1) {
    VOYAGER_LOG(ERROR) << "fcntl(F_GETFL): " << strerror(errno);
  }

  if (on) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }

  if (::fcntl(fd_, F_SETFL, flags) == -1) {
    VOYAGER_LOG(ERROR) << "fcntl(F_SETFL): " << strerror(errno);
  }

  flags = ::fcntl(fd_, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  if (::fcntl(fd_, F_SETFD, flags) == -1) {
    VOYAGER_LOG(ERROR) << "fcntl(F_SETFD): " << strerror(errno);
  }
}

void BaseSocket::SetReuseAddr(bool on) const {
  int reuse= on ? 1 : 0;
  if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, 
                   &reuse, static_cast<socklen_t>(sizeof(reuse))) == -1) {
    VOYAGER_LOG(ERROR) << "setsockopt(SO_REUSEADDR): " << strerror(errno);
  }
}

void BaseSocket::SetReusePort(bool on) const {
#ifdef SO_REUSEPORT
  int reuse = on ? 1 : 0;
  if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, 
                   &reuse, static_cast<socklen_t>(sizeof(reuse))) == -1) {
    VOYAGER_LOG(ERROR) << "setsockopt(SO_REUSEPORT): " << strerror(errno);
  }
#endif
}

void BaseSocket::SetKeepAlive(bool on) const {
  int alive = on ? 1 : 0;
  if (::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, 
                   &alive, static_cast<socklen_t>(sizeof(alive))) == -1) {
    VOYAGER_LOG(ERROR) << "setsockopt(SO_KEEPALIVE): " << strerror(errno);
  } 
}

void BaseSocket::SetTcpNoDelay(bool on) const {
  int no = on ? 1 : 0;
  if (::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, 
                   &no, static_cast<socklen_t>(sizeof(no))) == -1) {
    VOYAGER_LOG(ERROR) << "setsockopt(TCP_NODELAY): " << strerror(errno);
  } 
}

Status BaseSocket::CheckSocketError() const {
  int err = 0;
  socklen_t errlen = static_cast<socklen_t>(sizeof(err));
  if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR,
                   &err, &errlen) == -1) {
    VOYAGER_LOG(ERROR) << "getsockopt(SO_ERROR): " << strerror(errno);
  }
  if (err) {
    errno = err;
    return Status::IOError(strerror(errno));
  }
  return Status::OK();
}

struct sockaddr_storage BaseSocket::PeerSockAddr() const {
  struct sockaddr_storage sa;
  socklen_t len = sizeof(sa);

  if (::getpeername(fd_, 
                    reinterpret_cast<struct sockaddr*>(&sa), &len) == -1) {
    VOYAGER_LOG(ERROR) << "getpeername: " << strerror(errno);
  }
  return sa;
}

struct sockaddr_storage BaseSocket::LocalSockAddr() const {
  struct sockaddr_storage sa;
  socklen_t len = sizeof(sa);
  if (::getsockname(fd_, 
                    reinterpret_cast<struct sockaddr*>(&sa), &len) == -1) {
    VOYAGER_LOG(ERROR) << "getsockname: " << strerror(errno);
  }
  return sa;
}

int BaseSocket::IsSelfConnect() const {
  struct sockaddr_storage localaddr = LocalSockAddr();
  struct sockaddr_storage peeraddr = PeerSockAddr();
  return memcmp(&localaddr, &peeraddr, sizeof(sockaddr_storage));
}

}  // namespace voyager
