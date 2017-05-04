// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_SOCKADDR_H_
#define VOYAGER_CORE_SOCKADDR_H_

#include <netdb.h>
#include <string>

namespace voyager {

class SockAddr {
 public:
  explicit SockAddr(uint16_t port);
  SockAddr(const std::string& host, uint16_t port);

  const struct sockaddr* GetSockAddr() const {
    return reinterpret_cast<const struct sockaddr*>(&sa_);
  }
  sa_family_t Family() const { return sa_.ss_family; }
  std::string Ipbuf() const { return ipbuf_; }
  std::string Ip() const { return ip_; }
  uint16_t Port() const { return port_; }

  static bool SockAddrToIP(const struct sockaddr* sa,
                           char* buf, size_t len);
  static bool IPPortToSockAddr(const char* ip, uint16_t port,
                               struct sockaddr_in* sa4);
  static bool IPPortToSockAddr(const char* ip, uint16_t port,
                               struct sockaddr_in6* sa6);

  static struct sockaddr_storage LocalSockAddr(int socketfd);
  static struct sockaddr_storage PeerSockAddr(int socketfd);

  static int FormatLocal(int socketfd, char* buf, size_t len);
  static int FormatPeer(int socketfd, char* buf, size_t len);
  static int FormatAddress(const struct sockaddr* sa,
                           char* buf, size_t len);
  static int FormatAddress(const char* ip, uint16_t port,
                           char* buf, size_t len);

 private:
  bool GetAddrInfo(const char* host, uint16_t port);

  struct sockaddr_storage sa_;
  std::string ip_;
  uint16_t port_;
  std::string ipbuf_;
};

}  // namespace voyager

#endif  // VOYAGER_CORE_SOCKADDR_H_
