#ifndef VOYAGER_CORE_SOCKET_UTIL_H_
#define VOYAGER_CORE_SOCKET_UTIL_H_

#include <stdint.h>
#include <sys/uio.h>
#include <netinet/in.h>

#include "voyager/util/status.h"

namespace voyager {
namespace sockets {

ssize_t Read(int socketfd, void* buf, size_t count);
ssize_t ReadV(int socketfd, const struct iovec* iov, int count);
ssize_t Write(int socketfd, const void* buf, size_t count);
ssize_t WriteV(int socketfd, const struct iovec* iov, int count);

int FormatAddr(const char* ip, uint16_t port, char* buf, size_t buf_size);

void SockAddrToIP(const struct sockaddr* sa, char* ipbuf, size_t ipbuf_size);
int SockAddrToIPPort(const struct sockaddr* sa, char* buf, size_t buf_size);
void IPPortToSockAddr(const char* ip, uint16_t port, struct sockaddr_in* sa4);
void IPPortToSockAddr(const char* ip, uint16_t port, struct sockaddr_in6* sa6);

}  // namespace sockets
}  // namespace voyager 

#endif  // VOYAGER_CORE_SOCKET_UTIL_H_
