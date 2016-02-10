#ifndef MIRANTS_CORE_SOCKADDR_H_
#define MIRANTS_CORE_SOCKADDR_H_

#include <string>
#include <netdb.h>
#include "util/status.h"

namespace mirants {

class SockAddr {
 public:
  explicit SockAddr(uint16_t port);
  SockAddr(const std::string& host, uint16_t port);

  const struct addrinfo* AddrInfo() const { return addrinfo_; }
  std::string IP() const { return ip_; }

  void FreeAddrinfo();

 private:
  Status GetAddrInfo(const char* host, uint16_t port);

  struct addrinfo *addrinfo_;
  std::string ip_;
};

}  // namespace mirants 

#endif  // MIRANTS_CORE_SOCKADDR_H_
