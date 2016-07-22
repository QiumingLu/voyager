#include "voyager/core/socket_util.h"

#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <utility>

#include "voyager/util/logging.h"
#include "voyager/util/stringprintf.h"

namespace voyager {
namespace sockets{

ssize_t Read(int socketfd, void* buf, size_t count) {
  return ::read(socketfd, buf, count);
}

ssize_t ReadV(int socketfd, const struct iovec* iov, int count) {
  return ::readv(socketfd, iov, count);
}

ssize_t Write(int socketfd, const void* buf, size_t count) {
  return ::write(socketfd, buf, count);
}

ssize_t WriteV(int socketfd, const struct iovec* iov, int count) {
  return ::writev(socketfd, iov, count);
}

int FormatAddr(const char* ip, uint16_t port, char* buf, size_t buf_size) {
  return snprintf(buf, buf_size, strchr(ip, ':') ?
                  "[%s]:%d" : "%s:%u", ip, port);
}

void SockAddrToIP(const struct sockaddr* sa, char* ipbuf, size_t ipbuf_size) {
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

int SockAddrToIPPort(const struct sockaddr* sa, char* buf, size_t buf_size) {
  char ip[INET6_ADDRSTRLEN];
  SockAddrToIP(sa, ip, sizeof(ip));
  const struct sockaddr_in* sa4 = 
      reinterpret_cast<const struct sockaddr_in*>(sa);
  uint16_t port = ntohs(sa4->sin_port);
  return FormatAddr(ip, port, buf, buf_size);
}

void IPPortToSockAddr(const char* ip, uint16_t port, struct sockaddr_in* sa4) {
  sa4->sin_family = AF_INET;
  sa4->sin_port = htons(port);
  if (::inet_pton(AF_INET, ip, &sa4->sin_addr) <= 0) {
    VOYAGER_LOG(ERROR) << "inet_pton failed";
  }
}

void IPPortToSockAddr(const char* ip, 
                      uint16_t port, 
                      struct sockaddr_in6* sa6) {
  sa6->sin6_family = AF_INET6;
  sa6->sin6_port = htons(port);
  if (::inet_pton(AF_INET6, ip, &sa6->sin6_addr) <= 0) {
    VOYAGER_LOG(ERROR) << "inet_pton failed";
  }
}


}  // namespace sockets
}  // namespace voyager
