// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_SERVER_SOCKET_H_
#define VOYAGER_CORE_SERVER_SOCKET_H_

#include "voyager/core/base_socket.h"

namespace voyager {

class ServerSocket : public BaseSocket {
 public:
  ServerSocket(int domain, bool nonblocking)
      : BaseSocket(domain, nonblocking) {
  }

  explicit ServerSocket(int socketfd)
      : BaseSocket(socketfd) {
  }

  void Bind(const struct sockaddr* sa, socklen_t salen);
  void Listen(int backlog);
  int Accept(struct sockaddr* sa, socklen_t* salen);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_SERVER_SOCKET_H_
