// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/base_socket.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include "voyager/util/logging.h"

namespace voyager {

BaseSocket::BaseSocket(int domain, bool nonblocking)
    : fd_(::socket(domain, SOCK_STREAM, IPPROTO_TCP)), need_close_(true) {
  if (fd_ == -1) {
    VOYAGER_LOG(FATAL) << "socket: " << strerror(errno);
  }
  SetNonBlockAndCloseOnExec(nonblocking);
}

BaseSocket::BaseSocket(int socketfd) : fd_(socketfd), need_close_(true) {}

BaseSocket::~BaseSocket() {
  if (need_close_) {
    if (::close(fd_) == -1) {
      VOYAGER_LOG(ERROR) << "close: " << strerror(errno);
    }
  }
}

int BaseSocket::ShutDownWrite() const {
  if (::shutdown(fd_, SHUT_WR) == -1) {
    VOYAGER_LOG(ERROR) << "shutdown: " << strerror(errno);
    return -1;
  }
  return 0;
}

int BaseSocket::SetNonBlockAndCloseOnExec(bool on) const {
  int flags = ::fcntl(fd_, F_GETFL, 0);
  if (flags == -1) {
    VOYAGER_LOG(ERROR) << "fcntl(F_GETFL): " << strerror(errno);
    return -1;
  }

  if (on) {
    flags |= O_NONBLOCK;
  } else {
    flags &= ~O_NONBLOCK;
  }

  if (::fcntl(fd_, F_SETFL, flags) == -1) {
    VOYAGER_LOG(ERROR) << "fcntl(F_SETFL): " << strerror(errno);
    return -1;
  }

  flags = ::fcntl(fd_, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  if (::fcntl(fd_, F_SETFD, flags) == -1) {
    VOYAGER_LOG(ERROR) << "fcntl(F_SETFD): " << strerror(errno);
    return -1;
  }
  return 0;
}

int BaseSocket::SetReuseAddr(bool on) const {
  int reuse = on ? 1 : 0;
  if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse,
                   static_cast<socklen_t>(sizeof(reuse))) == -1) {
    VOYAGER_LOG(ERROR) << "setsockopt(SO_REUSEADDR): " << strerror(errno);
    return -1;
  }
  return 0;
}

int BaseSocket::SetReusePort(bool on) const {
#ifdef SO_REUSEPORT
  int reuse = on ? 1 : 0;
  if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &reuse,
                   static_cast<socklen_t>(sizeof(reuse))) == -1) {
    VOYAGER_LOG(ERROR) << "setsockopt(SO_REUSEPORT): " << strerror(errno);
    return -1;
  }
#endif
  return 0;
}

int BaseSocket::SetKeepAlive(bool on) const {
  int alive = on ? 1 : 0;
  if (::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &alive,
                   static_cast<socklen_t>(sizeof(alive))) == -1) {
    VOYAGER_LOG(ERROR) << "setsockopt(SO_KEEPALIVE): " << strerror(errno);
    return -1;
  }
  return 0;
}

int BaseSocket::SetTcpNoDelay(bool on) const {
  int no = on ? 1 : 0;
  if (::setsockopt(fd_, IPPROTO_TCP, TCP_NODELAY, &no,
                   static_cast<socklen_t>(sizeof(no))) == -1) {
    VOYAGER_LOG(ERROR) << "setsockopt(TCP_NODELAY): " << strerror(errno);
    return -1;
  }
  return 0;
}

int BaseSocket::CheckSocketError() const {
  int err = 0;
  socklen_t errlen = static_cast<socklen_t>(sizeof(err));
  if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
    VOYAGER_LOG(ERROR) << "getsockopt(SO_ERROR): " << strerror(errno);
  }
  return err;
}

struct sockaddr_storage BaseSocket::PeerSockAddr() const {
  struct sockaddr_storage sa;
  socklen_t len = sizeof(sa);
  if (::getpeername(fd_, reinterpret_cast<struct sockaddr*>(&sa), &len) == -1) {
    VOYAGER_LOG(ERROR) << "getpeername: " << strerror(errno);
  }
  return sa;
}

struct sockaddr_storage BaseSocket::LocalSockAddr() const {
  struct sockaddr_storage sa;
  socklen_t len = sizeof(sa);
  if (::getsockname(fd_, reinterpret_cast<struct sockaddr*>(&sa), &len) == -1) {
    VOYAGER_LOG(ERROR) << "getsockname: " << strerror(errno);
  }
  return sa;
}

bool BaseSocket::IsSelfConnect() const {
  struct sockaddr_storage localaddr = LocalSockAddr();
  struct sockaddr_storage peeraddr = PeerSockAddr();
  return memcmp(&localaddr, &peeraddr, sizeof(sockaddr_storage)) == 0;
}

}  // namespace voyager
