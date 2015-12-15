#include "core/sockaddr.h"

#include <netdb.h>
#include "core/socket_util.h"

namespace mirants {

SockAddr::SockAddr(const Slice& ipbuf, uint16_t port, bool ipv6) {
  if (ipv6) {
    memset(sa4_, 0, sizeof(sa4_));
    sockets::IPPortToSockAddr(ipbuf.data(), port, sa4_);
  } else {
    memset(sa6_, 0, sizeof(sa6_));
    sockets::IPPortToSockAddr(ipbuf.data(), port, sa6_);
  }
}

Slice SockAddr::ToIP() const {
  char ipbuf[INET6_ADDRSTRLEN];
  sockets::SockAddrToIP(getSockAddr(), ipbuf, sizeof(ipbuf));
  return Slice(ipbuf);
}

Slice SockAddr::ToIPPort() const {
  char buf[INET6_ADDRSTRLEN];
  sockets::SockAddrToIPPort(getSockAddr(), buf, sizeof(buf));
  return Slice(buf);
}

}  // namespace mirants
