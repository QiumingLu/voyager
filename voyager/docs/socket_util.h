#ifndef VOYAGER_CORE_SOCKET_UTIL_H_
#define VOYAGER_CORE_SOCKET_UTIL_H_

#include <stdint.h>
#include <sys/uio.h>
#include <netinet/in.h>

#include "voyager/util/status.h"

namespace voyager {
namespace sockets {
  
// int CreateSocket(int domain);
// int CreateSocketAndSetNonBlock(int domain);
// void CloseFd(int socketfd);
// void BindAddress(int socketfd, const struct sockaddr* sa, socklen_t salen);
// void Listen(int socketfd, int backlog);
// int Accept(int socketfd, struct sockaddr* sa, socklen_t* salen);
// int Connect(int socketfd, const struct sockaddr* sa, socklen_t salen);

ssize_t Read(int socketfd, void* buf, size_t count);
ssize_t ReadV(int socketfd, const struct iovec* iov, int count);
ssize_t Write(int socketfd, const void* buf, size_t count);
ssize_t WriteV(int socketfd, const struct iovec* iov, int count);
// void ShutDownWrite(int socketfd);

// Status SetBlockingAndCloseOnExec(int socketfd, bool blocking);
// Status SetReuseAddr(int socketfd, bool reuse);
// Status SetReusePort(int socketfd, bool reuse);
// Status SetKeepAlive(int socketfd, bool alive);
// Status SetTcpNoDelay(int socketfd, bool notdelay);
// Status CheckSocketError(int socketfd);

// Status Resolve(char* hostname, char* ipbuf, size_t ipbuf_size);
// Status ResolveIP(char* hostname, char* ipbuf, size_t ipbuf_size);

// int FormatAddr(const char* ip, uint16_t port, char* buf, size_t buf_size);
// int FormatPeer(int socketfd, char* buf, size_t buf_size);
// int FormatLocal(int socketfd, char* buf, size_t buf_size);

// void SockAddrToIP(const struct sockaddr* sa, char* ipbuf, size_t ipbuf_size);
// int SockAddrToIPPort(const struct sockaddr* sa, char* buf, size_t buf_size);
// void IPPortToSockAddr(const char* ip, uint16_t port, struct sockaddr_in* sa4);
// void IPPortToSockAddr(const char* ip, uint16_t port, struct sockaddr_in6* sa6);

// struct sockaddr_storage PeerSockAddr(int socketfd);
// struct sockaddr_storage LocalSockAddr(int socketfd);
// int IsSelfConnect(int socketfd);

}  // namespace sockets
}  // namespace voyager 

#endif  // VOYAGER_CORE_SOCKET_UTIL_H_
