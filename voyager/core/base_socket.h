// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_BASE_SOCKET_H_
#define VOYAGER_CORE_BASE_SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

namespace voyager {

class BaseSocket {
 public:
  // RAII create a new socket fd.
  BaseSocket(int domain, bool nonblocking);
  // RAII manage a created socket fd.
  explicit BaseSocket(int socketfd);
  ~BaseSocket();

  int SocketFd() const { return fd_; }

  int ShutDownWrite() const;
  int SetNonBlockAndCloseOnExec(bool on) const;
  int SetReuseAddr(bool on) const;
  int SetReusePort(bool on) const;
  int SetKeepAlive(bool on) const;
  int SetTcpNoDelay(bool on) const;

  int CheckSocketError() const;

  struct sockaddr_storage PeerSockAddr() const;
  struct sockaddr_storage LocalSockAddr() const;
  bool IsSelfConnect() const;

  void SetNoAutoCloseFd() { need_close_ = false; }

 protected:
  const int fd_;

 private:
  bool need_close_;

  // No copying allowed
  BaseSocket(const BaseSocket&);
  void operator=(const BaseSocket&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_BASE_SOCKET_H_
