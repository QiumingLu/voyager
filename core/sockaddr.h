#ifndef MIRANTS_CORE_SOCKADDR_H_
#define MIRANTS_CORE_SOCKADDR_H_

#include <netdb.h>
#include "util/slice.h"
#include "util/status.h"

namespace mirants {

class SockAddr {
 public:
  explicit SockAddr(uint16_t port, bool ipv6 = false);
  SockAddr(Slice host, uint16_t port, bool ipv6 = false);

  const struct addrinfo* AddInfo() const { return addinfo_; }
  bool IsIpv6() const { return ipv6_; }
 private:
  Status GetAddrInfo(const char* host, uint16_t port);

  Slice host_;
  uint16_t port_;
  bool ipv6_;
  struct addrinfo *addinfo_;

  // No copying allow
  SockAddr(const SockAddr&);
  void operator=(const SockAddr&);
};

}  // namespace mirants 

#endif  // MIRANTS_CORE_SOCKADDR_H_
