#ifndef MIRANTS_CORE_SOCKADDR_H_
#define MIRANTS_CORE_SOCKADDR_H_

#include "util/slice.h"

namespace mirants {

class SockAddr {
 public:
  SockAddr(const Slice& ipbuf, uint16_t port, bool ipv6 = false);
  explicit SockAddr(const struct sockaddr_in& sa4) : sa4_(sa4) { }
  explicit SockAddr(const struct sockaddr_in6& sa6) : sa6_(sa6) { }

  const struct sockaddr* getSockAddr() { return static_cast<struct sockaddr*>(&sockaddr_in6); }

  Slice ToIP() const;
  Slice ToIPPort() const;

 private:
  union {
    struct sockaddr_in  sa4_;
    struct sockaddr_in6 sa6_;
  };

  // No copying allow
  SockAddr(const SockAddr&);
  void operator=(const SockAddr&);
};

}  // namespace mirants 

#endif  // MIRANTS_CORE_SOCKADDR_H_
