#include "core/sockaddr.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include "core/socket_util.h"
#include "util/stringprintf.h"
#include "util/logging.h"

namespace mirants {

SockAddr::SockAddr(uint16_t port) : addrinfo_(NULL) {
  Status st = GetAddrInfo(NULL, port);
  if (!st.ok()) {
    MIRANTS_LOG(ERROR) << st;
  }
}

SockAddr::SockAddr(const std::string& host, uint16_t port) {
  Status st = GetAddrInfo(host.c_str(), port);
  if (!st.ok()) {
    MIRANTS_LOG(ERROR) << st;
  }
}

void SockAddr::FreeAddrinfo() {
  if (addrinfo_ != NULL) {
    ::freeaddrinfo(addrinfo_);
    addrinfo_ = NULL;
  }
}

Status SockAddr::GetAddrInfo(const char* host, uint16_t port) {
  char portbuf[6];
  struct addrinfo hints;

  snprintf(portbuf, sizeof(portbuf), "%u", port);
  memset(&hints, 0, sizeof(hints));
  // hints.ai_family = ipv6 ? AF_INET6 : AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;  // No effect if bindaddr != NULL */

  int ret = ::getaddrinfo(host, portbuf, &hints, &addrinfo_);
  if (ret != 0) {
    std::string str;
    StringAppendF(&str, "getaddrinfo: %s", gai_strerror(ret));
    return Status::IOError(str);
  }

  char ipbuf[64];
  if (addrinfo_->ai_family == AF_INET) {
    assert(sizeof(ipbuf) >= INET_ADDRSTRLEN);
    struct sockaddr_in* sa4 = 
        reinterpret_cast<struct sockaddr_in*>(addrinfo_->ai_addr);
    ::inet_ntop(AF_INET, 
                &(sa4->sin_addr), 
                ipbuf, 
                static_cast<socklen_t>(sizeof(ipbuf)));
  } else if (addrinfo_->ai_family == AF_INET6){
    assert(sizeof(ipbuf) >= INET6_ADDRSTRLEN);
    struct sockaddr_in6* sa6 =
        reinterpret_cast<struct sockaddr_in6*>(addrinfo_->ai_addr);
    ::inet_ntop(AF_INET6, 
                &(sa6->sin6_addr), 
                ipbuf, 
                static_cast<socklen_t>(sizeof(ipbuf)));
  }

  ip_ = strchr(ipbuf, ':') ? 
      StringPrintf("[%s:%s]", ipbuf, portbuf) 
      : StringPrintf("%s:%s", ipbuf, portbuf);

  return Status::OK();
}

}  // namespace mirants
