// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/sockaddr.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "voyager/util/stringprintf.h"
#include "voyager/util/logging.h"

namespace voyager {

SockAddr::SockAddr(uint16_t port) {
  Status st = GetAddrInfo("127.0.0.1", port);
  if (!st.ok()) {
    VOYAGER_LOG(ERROR) << st;
  }
}

SockAddr::SockAddr(const std::string& host, uint16_t port) {
  Status st = GetAddrInfo(host.c_str(), port);
  if (!st.ok()) {
    VOYAGER_LOG(ERROR) << st;
  }
}

Status SockAddr::GetAddrInfo(const char* host, uint16_t port) {
  char portbuf[6];
  struct addrinfo hints, *result;

  snprintf(portbuf, sizeof(portbuf), "%u", port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;  // No effect if bindaddr != nullptr */

  int ret = ::getaddrinfo(host, portbuf, &hints, &result);
  if (ret != 0) {
    std::string str;
    StringAppendF(&str, "getaddrinfo: %s", gai_strerror(ret));
    return Status::IOError(str);
  }

  memcpy(&this->sa_, result->ai_addr, result->ai_addrlen);
  ::freeaddrinfo(result);

  char ipbuf[64];
  SockAddr::SockAddrToIPPort(reinterpret_cast<sockaddr*>(&sa_),
                             ipbuf, sizeof(ipbuf));
  ipbuf_ = std::string(ipbuf, sizeof(ipbuf));

  return Status::OK();
}

int SockAddr::FormatAddress(const char* ip, uint16_t port,
                            char* buf, size_t buf_size) {
  return snprintf(buf, buf_size, strchr(ip, ':') ?
                  "[%s]:%d" : "%s:%u", ip, port);
}

void SockAddr::SockAddrToIP(const struct sockaddr* sa,
                            char* ipbuf, size_t ipbuf_size) {
  if (sa->sa_family == AF_INET) {
    assert(ipbuf_size >= INET_ADDRSTRLEN);
    const struct sockaddr_in* sa4 =
        reinterpret_cast<const struct sockaddr_in*>(sa);
    ::inet_ntop(AF_INET, &sa4->sin_addr,
                ipbuf, static_cast<socklen_t>(ipbuf_size));
  } else if (sa->sa_family == AF_INET6) {
    assert(ipbuf_size >= INET6_ADDRSTRLEN);
    const struct sockaddr_in6* sa6 =
        reinterpret_cast<const struct sockaddr_in6*>(sa);
    ::inet_ntop(AF_INET6, &sa6->sin6_addr,
                ipbuf, static_cast<socklen_t>(ipbuf_size));
  }
}

int SockAddr::SockAddrToIPPort(const struct sockaddr* sa,
                               char* buf, size_t buf_size) {
  char ip[INET6_ADDRSTRLEN];
  SockAddrToIP(sa, ip, sizeof(ip));
  const struct sockaddr_in* sa4 =
      reinterpret_cast<const struct sockaddr_in*>(sa);
  uint16_t port = ntohs(sa4->sin_port);
  return FormatAddress(ip, port, buf, buf_size);
}

void SockAddr::IPPortToSockAddr(const char* ip, uint16_t port,
                                struct sockaddr_in* sa4) {
  sa4->sin_family = AF_INET;
  sa4->sin_port = htons(port);
  if (::inet_pton(AF_INET, ip, &sa4->sin_addr) <= 0) {
    VOYAGER_LOG(ERROR) << "inet_pton failed";
  }
}

void SockAddr::IPPortToSockAddr(const char* ip, uint16_t port,
                                struct sockaddr_in6* sa6) {
  sa6->sin6_family = AF_INET6;
  sa6->sin6_port = htons(port);
  if (::inet_pton(AF_INET6, ip, &sa6->sin6_addr) <= 0) {
    VOYAGER_LOG(ERROR) << "inet_pton failed";
  }
}

struct sockaddr_storage SockAddr::LocalSockAddr(int socketfd) {
  struct sockaddr_storage sa;
  socklen_t salen = sizeof(sa);
  if (::getsockname(socketfd,
                    reinterpret_cast<struct sockaddr*>(&sa), &salen) == -1) {
    VOYAGER_LOG(ERROR) << "getsockname failed";
  }
  return sa;
}

struct sockaddr_storage SockAddr::PeerSockAddr(int socketfd) {
  struct sockaddr_storage sa;
  socklen_t salen = sizeof(sa);

  if (::getpeername(socketfd,
                    reinterpret_cast<struct sockaddr*>(&sa), &salen) == -1) {
    VOYAGER_LOG(ERROR) << "getpeername failed";
  }
  return sa;
}

int SockAddr::FormatPeer(int socketfd, char* buf, size_t buf_size) {
  struct sockaddr_storage sa(PeerSockAddr(socketfd));
  return SockAddrToIPPort(reinterpret_cast<struct sockaddr*>(&sa),
                          buf,
                          buf_size);
}

int SockAddr::FormatLocal(int socketfd, char* buf, size_t buf_size) {
  struct  sockaddr_storage sa(LocalSockAddr(socketfd));
  return SockAddrToIPPort(reinterpret_cast<struct sockaddr*>(&sa),
                          buf,
                          buf_size);
}

}  // namespace voyager
