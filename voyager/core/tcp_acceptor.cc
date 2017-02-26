// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/tcp_acceptor.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "voyager/core/sockaddr.h"
#include "voyager/core/eventloop.h"

namespace voyager {

TcpAcceptor::TcpAcceptor(EventLoop* eventloop,
                         const SockAddr& addr,
                         int backlog,
                         bool reuseport)
    : eventloop_(eventloop),
      socket_(addr.Family(), true),
      dispatch_(eventloop_, socket_.SocketFd()),
      backlog_(backlog),
      idlefd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)),
      listenning_(false) {
  assert(idlefd_ >= 0);
  socket_.SetReuseAddr(true);
  socket_.SetReusePort(reuseport);
  socket_.Bind(addr.GetSockAddr(), sizeof(*(addr.GetSockAddr())));
  dispatch_.SetReadCallback(std::bind(&TcpAcceptor::Accept, this));
}

TcpAcceptor::~TcpAcceptor() {
  dispatch_.DisableAll();
  dispatch_.RemoveEvents();
  ::close(idlefd_);
}

void TcpAcceptor::EnableListen() {
  eventloop_->AssertInMyLoop();
  listenning_ = true;
  socket_.Listen(backlog_);
  dispatch_.EnableRead();
}

void TcpAcceptor::Accept() {
  eventloop_->AssertInMyLoop();
  struct sockaddr_storage sa;
  socklen_t salen = static_cast<socklen_t>(sizeof(sa));
  int connectfd = socket_.Accept(reinterpret_cast<struct sockaddr*>(&sa),
                                 &salen);
  if (connectfd >= 0) {
    if (conn_cb_) {
      conn_cb_(connectfd, sa);
    } else {
      ::close(connectfd);
    }
  } else {
    if (errno == EMFILE) {
      ::close(idlefd_);
      idlefd_ = ::accept(socket_.SocketFd(), nullptr, nullptr);
      ::close(idlefd_);
      idlefd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
    }
  }
}

}  // namespace voyager
