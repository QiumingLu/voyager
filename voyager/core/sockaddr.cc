// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/sockaddr.h"

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "voyager/util/logging.h"

namespace voyager {

SockAddr::SockAddr(uint16_t port) {
  GetAddrInfo("127.0.0.1", port);
}

SockAddr::SockAddr(const std::string& host, uint16_t port) {
  GetAddrInfo(host.c_str(), port);
}

bool SockAddr::GetAddrInfo(const char* host, uint16_t port) {
  char portbuf[6];
  struct addrinfo hints, *result;

  snprintf(portbuf, sizeof(portbuf), "%u", port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;  // No effect if bindaddr != nullptr

  int ret = ::getaddrinfo(host, portbuf, &hints, &result);
  if (ret != 0) {
    VOYAGER_LOG(ERROR) << "host:" << host << " port:" << port
                       << " getaddrinfo: " << gai_strerror(ret);
    return false;
  }

  memcpy(&this->sa_, result->ai_addr, result->ai_addrlen);
  ::freeaddrinfo(result);

  char ip[64];
  SockAddr::SockAddrToIP(reinterpret_cast<sockaddr*>(&sa_),
                         ip, sizeof(ip));

  char ipbuf[128];
  FormatAddress(ip, port, ipbuf, sizeof(ipbuf));

  ip_ = std::string(ip);
  port_ = port;
  ipbuf_ = std::string(ipbuf, sizeof(ipbuf));

  return true;
}

bool SockAddr::SockAddrToIP(const struct sockaddr* sa,
                            char* buf, size_t len) {
  if (sa->sa_family == AF_INET) {
    assert(len >= INET_ADDRSTRLEN);
    const struct sockaddr_in* sa4 =
        reinterpret_cast<const struct sockaddr_in*>(sa);
    if (::inet_ntop(AF_INET, &sa4->sin_addr,
                    buf, static_cast<socklen_t>(len)) != nullptr) {
      return true;
    }
  } else if (sa->sa_family == AF_INET6) {
    assert(len >= INET6_ADDRSTRLEN);
    const struct sockaddr_in6* sa6 =
        reinterpret_cast<const struct sockaddr_in6*>(sa);
    if (::inet_ntop(AF_INET6, &sa6->sin6_addr,
                    buf, static_cast<socklen_t>(len)) != nullptr) {
      return true;
    }
  }
  return false;
}

bool SockAddr::IPPortToSockAddr(const char* ip, uint16_t port,
                                struct sockaddr_in* sa4) {
  sa4->sin_family = AF_INET;
  sa4->sin_port = htons(port);
  if (::inet_pton(AF_INET, ip, &sa4->sin_addr) <= 0) {
    VOYAGER_LOG(ERROR) << "inet_pton: " << strerror(errno);
    return false;
  }
  return true;
}

bool SockAddr::IPPortToSockAddr(const char* ip, uint16_t port,
                                struct sockaddr_in6* sa6) {
  sa6->sin6_family = AF_INET6;
  sa6->sin6_port = htons(port);
  if (::inet_pton(AF_INET6, ip, &sa6->sin6_addr) <= 0) {
    VOYAGER_LOG(ERROR) << "inet_pton: " << strerror(errno);
    return false;
  }
  return true;
}

struct sockaddr_storage SockAddr::LocalSockAddr(int socketfd) {
  struct sockaddr_storage sa;
  socklen_t salen = sizeof(sa);
  if (::getsockname(socketfd,
                    reinterpret_cast<struct sockaddr*>(&sa), &salen) == -1) {
    VOYAGER_LOG(ERROR) << "getsockname: " << strerror(errno);
  }
  return sa;
}

struct sockaddr_storage SockAddr::PeerSockAddr(int socketfd) {
  struct sockaddr_storage sa;
  socklen_t salen = sizeof(sa);

  if (::getpeername(socketfd,
                    reinterpret_cast<struct sockaddr*>(&sa), &salen) == -1) {
    VOYAGER_LOG(ERROR) << "getpeername: " << strerror(errno);
  }
  return sa;
}

int SockAddr::FormatPeer(int socketfd, char* buf, size_t len) {
  struct sockaddr_storage sa(PeerSockAddr(socketfd));
  return FormatAddress(reinterpret_cast<struct sockaddr*>(&sa), buf, len);
}

int SockAddr::FormatLocal(int socketfd, char* buf, size_t len) {
  struct sockaddr_storage sa(LocalSockAddr(socketfd));
  return FormatAddress(reinterpret_cast<struct sockaddr*>(&sa), buf, len);
}

int SockAddr::FormatAddress(const struct sockaddr* sa,
                             char* buf, size_t len) {
  char ip[INET6_ADDRSTRLEN];
  bool res = SockAddrToIP(sa, ip, sizeof(ip));
  if (!res) {
    return -1;
  }
  const struct sockaddr_in* sa4 =
      reinterpret_cast<const struct sockaddr_in*>(sa);
  uint16_t port = ntohs(sa4->sin_port);
  return FormatAddress(ip, port, buf, len);
}

int SockAddr::FormatAddress(const char* ip, uint16_t port,
                            char* buf, size_t len) {
  return snprintf(buf, len, strchr(ip, ':') ? "[%s]:%u" : "%s:%u", ip, port);
}

}  // namespace voyager
