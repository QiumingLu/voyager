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

  for (p = servinfo_; p != NULL; p = p->ai_next) {
    int socketfd = sockets::CreateSocketAndSetNonBlock(p->ai_family);
    int ret = sockets::connect(socketfd, p0>ai_addr, p->ai_addrlen);
    int err = (ret == 0) ? 0 : errno;
    switch (err) {
      case 0:
      case EINPROGRESS:
      case EINTR:
        connecting(socketfd);
        break;

      case EAGAIN:
      case EADDRINUSE:
      case EADDRNOTAVAIL:
      case ECONNRESUSED:
      case ENETUNREACH:
        retry(socketfd);
        break;

      case EACESS:
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

