#include "core/sockaddr.h"

#include <sys/types.h>
#include <sys/socket.h>
#include "util/stringprintf.h"
#include "util/logging.h"

namespace mirants {

SockAddr::SockAddr(uint16_t port, bool ipv6) : port_(port), ipv6_(ipv6) {
  Status st = GetAddrInfo(NULL, port, ipv6_);
  if (!st.ok()) {
    MIRANTS_LOG(ERROR) << st;
  }
}

SockAddr::SockAddr(Slice host, uint16_t port, bool ipv6) 
    : host_(host),
      port_(port), 
      ipv6_(ipv6) {
  Status st = GetAddrInfo(host_.data(), port_, ipv6_);
  if (!st.ok()) {
    MIRANTS_LOG(ERROR) << st;
  }
}

SockAddr::~SockAddr() {
  ::freeaddrinfo(addrinfo_);
}

Status SockAddr::GetAddrInfo(const char* host, uint16_t port, bool ipv6) {
  char portstr[6];
  struct addrinfo hints;

  snprintf(portstr, sizeof(portstr), "%u", port);
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = ipv6 ? AF_INET6 : AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;  // No effect if bindaddr != NULL */

  int ret = ::getaddrinfo(host, &hints, &addrinfo_);
  if (ret != 0) {
    std::string str;
    StringAppendF(&str, "getaddrinfo: %s", gai_strerror(ret));
    return Status::IOError(str);
  }
  return Status::OK();
}

}  // namespace mirants
