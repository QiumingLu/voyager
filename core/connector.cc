#include "core/connector.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "core/socket_util.h"
#include "util/stringprintf.h"
#include "util/logging.h"

namespace mirants {

Connector::Connector(const struct addrinfo* addr) : servinfo_(addr) {
}

void Connector::TcpNonBlockConnect() {
  const struct  addrinfo* p;

  for (p = servinfo_; p != NULL; p = p->ai_next) {
    int socketfd = sockets::CreateSocketAndSetNonBlock(p->ai_family);
    int ret = sockets::Connect(socketfd, p->ai_addr, p->ai_addrlen);
    int err = (ret == 0) ? 0 : errno;
    switch (err) {
      case 0:
      case EINPROGRESS:
      case EINTR:
      //  connecting(socketfd);
        break;

      case EAGAIN:
      case EADDRINUSE:
      case EADDRNOTAVAIL:
      case ECONNREFUSED:
      case ENETUNREACH:
      //  retry(socketfd);
        break;

      case EACCES:
      case EPERM:
      case EAFNOSUPPORT:
      case EALREADY:
      case EBADF:
      case EFAULT:
      case ENOTSOCK:
        MIRANTS_LOG(ERROR) << "connect error: " << strerror(err);
        sockets::CloseFd(socketfd);
        break;

      default:
        MIRANTS_LOG(ERROR) << "unexpected connect error: " << strerror(err);
        sockets::CloseFd(socketfd);
        break;
    }
  }
}

}  // namespace mirants

