#include "core/socket_util.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "util/logging.h"

namespace mirants {
namespace SocketsUtil{

int CreateSocket(int domain) {
  int socketfd = ::socket(domain, SOCK_STREAM, IPPROTO_TCP);
  if (socketfd == -1) {
    MIRANTS_LOG(FATAL) << "SocketsUtil: create socket failed.";
  }
  return sockfd;
}

void CloseFd(int socketfd) {
  if (::close(socketfd) == -1) {
    MIRANTS_LOG(ERROR) << "SocketsUtil: close file descriptor error.";
  }
}

void BindAddress(int socketfd, const struct sockaddr_in* addr) {
  int ret = ::bind(socketfd,
                   static_cast<const struct sockaddr*>(addr),
                   static_cast<socklen_t>(sizeof(*addr)));
  if (ret == -1) {
    MIRANTS_LOG(FATAL) << "SocketsUtil: bind failed";
  }
}

void Listen(int socketfd) {
  int ret = ::listen(socketfd, SOMAXCONN);
  if (ret == -1) {
    MIRANTS_LOG(FATAL) << "SocketsUtil: listen failed";
  }
}

int Accept(int socketfd, struct sockaddr_in* addr) {
  int connectfd = ::accept(socketfd, 
                           static_cast<const struct sockaddr*>(addr),
                           static_cast<socklen_t>(sizeof(*addr)));
  if (connectfd == -1) {
    int err = errno;
    switch (err) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO: 
      case EPERM:
      case EMFILE: 
        MIRANTS_LOG(ERROR) << "SocketsUtil: Accept error " << err;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        MIRANTS_LOG(FATAL) << "SocketsUtil: Accept unexpected error " << err;
        break;
      default:
        MIRANTS_LOG(FATAL) << "SocketsUtil: Accept unkown error " << err;      
  }

  Status status = SetBlocking(connectfd, false);
  if (!status.ok()) {
    MIRANTS_LOG(ERROR) << status;
  }
}

int Connect(int socketfd, const struct sockaddr_in* addr) {
  int ret = ::connect(socketfd, 
                      static_cast<const struct sockaddr* >(addr), 
                      static_cast<socklen_t>(sizeof (*addr)));
  return ret;
}


Status SetBlocking(int socketfd, bool blocking) {
  int flags = ::fcntl(socketfd, F_GETFL, 0);
  if (flags == -1) {
    char buf[128];
    int len = snprintf(buf, sizeof(buf), 
                       "SocketsUtil: SetBlocking - fcntl(F_GETFL)");
    strerror_r(errno, buf + len, sizeof(buf) - len);
    CloseFd(socketfd);
    return Status::IOError(buf);
  }

  if (blocking) {
    flags &= ~O_NONBLOCK;
  } else {
    flags |= O_NONBLOCK;
  }

  if (::fcntl(socketfd, F_SETFL, flags) == -1) {
    char buf[128];
    int len = snprintf(buf, sizeof(buf), 
                       "SocketsUtil: SetBlocking - fcntl(F_GETFL)");
    strerror_r(errno, buf + len, sizeof(buf) - len);
    CloseFd(socketfd);
    return Status::IOError(buf);
  }
  return Status::OK();
}

Status SetReuseAddr(int sockfd, bool reuse) {
  int on = reuse ? 1 : 0;
  if (::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
                   &on, static_cast<scoklen_t>(sizeof(on))) == -1) {
    char buf[128];
    int len = snprintf(buf, sizeof(buf), 
                       "SocketsUtil: SetReuseAddr - setsockopt error");
    strerror_r(errno, buf + len, sizeof(buf) - len);
    CloseFd(sockfd);
    return Status::IOError(buf);
  }
  return Status::OK();
}


Status SetReusePort(int sockfd, bool reuse) {
#ifdef SO_REUSEPORT

  int on = reuse ? 1 : 0;
  if (::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, 
                   &on, static_cast<scoklen_t>(sizeof(on))) == -1) {
    char buf[128];
    int len = snprintf(buf, sizeof(buf), 
                       "SocketsUtil: SetReusePort - setsockopt error");
    strerror_r(errno, buf + len, sizeof(buf) - len);
    CloseFd(sockfd);
    return Status::IOError(buf);
  }
  return Status::OK();

#endif
}

Status SetKeepAlive(int sockfd, bool alive) {
  int on = alive ? 1 : 0;
  if (::setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, 
                   &on, static_cast<scoklen_t>(sizeof(on))) == -1) {
    char buf[128];
    int len = snprintf(buf, sizeof(buf), 
                       "SocketsUtil: SetKeepAlive - setsockopt error");
    strerror_r(errno, buf + len, sizeof(buf) - len);
    CloseFd(sockfd);
    return Status::IOError(buf);
  }
  return Status::OK();
}


Status SetTcpNotDelay(int sockfd, bool notdelay) {
  int on = notdelay ? 1 : 0;
  if (::setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, 
                   &on, static_cast<scoklen_t>(sizeof(on))) == -1) {
    char buf[128];
    int len = snprintf(buf, sizeof(buf), 
                       "SocketsUtil: SetTcpNotDelay - setsockopt error");
    strerror_r(errno, buf + len, sizeof(buf) - len);
    CloseFd(sockfd);
    return Status::IOError(buf);
  }
  return Status::OK();
}

Status CheckSocketError(int sockfd) {
  int err = 0;
  socklen_t errlen = static_cast<socklen_t>(sizeof(err));

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
    char buf[30];
    int len = snprintf(buf, sizeof(buf), 
                       "SocketsUtil: CheckSocketError - getsockopt (SO_ERROR) error");
    strerror_r(errno, buf + len, sizeof(buf) - len);
    return Status::IOError(buf);
  }
  if (err) {
    errno = err;
    char buf[30];
    strerror_r(errno, buf, sizeof(buf));
    return Status::IOError(buf);
  }
  return Status::OK();
}

}  // namespace SocketsUtil
