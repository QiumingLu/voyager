#ifndef MIRANTS_CORE_SOCKET_UTIL_H_
#define MIRANTS_CORE_SOCKET_UTIL_H_

#include <netinet/in.h>
#include "util/status.h"

namespace mirants {
namespace sockets {
  
int CreateSocket(int domain);

void CloseFd(int socketfd);

void BindAddress(int socketfd, const struct sockaddr_in* addr);

void Listen(int socketfd);

void Accept(int socketfd, struct sockaddr_in* addr);

int Connect(int socketfd, const struct sockaddr_in* addr);

Status SetBlocking(int socketfd, bool blocking);

Status SetReuseAddr(int sockfd, bool reuse);

Status CheckSocketError(int sockfd);

}  // namespace sockets
}  // namespace mirants 

#endif  // MIRANTS_CORE_SOCKET_UTIL_H_
