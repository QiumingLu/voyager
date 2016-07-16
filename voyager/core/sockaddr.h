#ifndef VOYAGER_CORE_SOCKADDR_H_
#define VOYAGER_CORE_SOCKADDR_H_

#include <string>
#include <netdb.h>
#include "voyager/util/status.h"

namespace voyager {

class SockAddr {
 public:
  explicit SockAddr(uint16_t port);
  SockAddr(const std::string& host, uint16_t port);

  const struct sockaddr* GetSockAddr () const { 
    return reinterpret_cast<const struct sockaddr*>(&sa_);
  }
  sa_family_t Family() const { return sa_.ss_family; }
  std::string IP() const { return ip_; }

 private:
  Status GetAddrInfo(const char* host, uint16_t port);

  struct sockaddr_storage sa_;
  std::string ip_;
};

}  // namespace voyager 

#endif  // VOYAGER_CORE_SOCKADDR_H_
