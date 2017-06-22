// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_CLIENT_SOCKET_H_
#define VOYAGER_CORE_CLIENT_SOCKET_H_

#include "voyager/core/base_socket.h"

namespace voyager {

class ClientSocket : public BaseSocket {
 public:
  ClientSocket(int domain, bool nonblocking)
      : BaseSocket(domain, nonblocking) {}

  explicit ClientSocket(int socketfd) : BaseSocket(socketfd) {}

  int Connect(const struct sockaddr* sa, socklen_t salen) const;

 private:
  // No copying allowed
  ClientSocket(const ClientSocket&);
  void operator=(const ClientSocket&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_CLIENT_SOCKET_H_
