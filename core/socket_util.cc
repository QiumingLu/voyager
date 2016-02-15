#include "core/socket_util.h"

#include <utility>
#include <assert.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "util/logging.h"
#include "util/stringprintf.h"

namespace mirants {
namespace sockets{

int CreateSocket(int domain) {
  int socketfd = ::socket(domain, SOCK_STREAM, IPPROTO_TCP);
  if (socketfd == -1) {
    MIRANTS_LOG(FATAL) << "socket: " << strerror(errno);
  }
  return socketfd;
}

int CreateSocketAndSetNonBlock(int domain) {
  int socketfd = CreateSocket(domain);
  Status st = SetBlockingAndCloseOnExec(socketfd, false);
  if (!st.ok()) {
    MIRANTS_LOG(FATAL) << st;
  }
  return socketfd;
}

void CloseFd(int socketfd) {
  if (::close(socketfd) == -1) {
    MIRANTS_LOG(ERROR) << "close: " << strerror(errno);
  }
}

void BindAddress(int socketfd, const struct sockaddr* sa, socklen_t salen) {
  int ret = ::bind(socketfd, sa, salen);
  if (ret == -1) {
    MIRANTS_LOG(FATAL) << "bind: " << strerror(errno);
  }
}

void Listen(int socketfd, int backlog) {
  int ret = ::listen(socketfd, backlog);
  if (ret == -1) {
    MIRANTS_LOG(FATAL) << "listen: " << strerror(errno);
  }
}

int Accept(int socketfd, struct sockaddr* sa, socklen_t* salen) {
  int connectfd = ::accept(socketfd, sa, salen);
  if (connectfd == -1) {
    int err = errno;
    switch (err) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO: 
      case EPERM:
      case EMFILE: 
        MIRANTS_LOG(ERROR) << "Accept: " << strerror(err);
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        MIRANTS_LOG(FATAL) << "Accept unexpected error: " << strerror(err);
        break;
      default:
        MIRANTS_LOG(FATAL) << "Accept unkown error " << strerror(err);      
    }
  }

  Status status = SetBlockingAndCloseOnExec(connectfd, false);
  if (!status.ok()) {
    MIRANTS_LOG(ERROR) << status;
  }
  return connectfd;
}

int Connect(int socketfd, const struct sockaddr* sa, socklen_t salen) {
  int ret = ::connect(socketfd, sa, salen);
  return ret;
}

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

void ShutDownWrite(int socketfd) {
  if (::shutdown(socketfd, SHUT_WR) == -1) {
    MIRANTS_LOG(ERROR) << "shutdown: " << strerror(errno);
  }
}

Status SetBlockingAndCloseOnExec(int socketfd, bool blocking) {
  int flags = ::fcntl(socketfd, F_GETFL, 0);
  if (flags == -1) {
    std::string str;
    StringAppendF(&str, "fcntl(F_GETFL): %s", strerror(errno));
    return Status::IOError(str);
  }

  if (blocking) {
    flags &= ~O_NONBLOCK;
  } else {
    flags |= O_NONBLOCK;
  }

  if (::fcntl(socketfd, F_SETFL, flags) == -1) {
    std::string str;
    StringAppendF(&str, "fcntl(F_SETFL, O_NONBLOCK): %s", strerror(errno));
    return Status::IOError(str);
  }

  flags = ::fcntl(socketfd, F_GETFD, 0);
  flags |= FD_CLOEXEC;
  if (::fcntl(socketfd, F_SETFD, flags) == -1) {
    std::string str;
    StringAppendF(&str, "fcntl(F_SETFD, FD_CLOEXEC): %s", strerror(errno));
    return Status::IOError(str);
  }

  return Status::OK();
}

Status SetReuseAddr(int socketfd, bool reuse) {
  int on = reuse ? 1 : 0;
  if (::setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, 
                   &on, static_cast<socklen_t>(sizeof(on))) == -1) {
    std::string str;
    StringAppendF(&str, "setsockopt SO_REUSEADDR: %s", strerror(errno));
    return Status::IOError(str);
  }
  return Status::OK();
}


Status SetReusePort(int socketfd, bool reuse) {
#ifdef SO_REUSEPORT
  int on = reuse ? 1 : 0;
  if (::setsockopt(socketfd, SOL_SOCKET, SO_REUSEPORT, 
                   &on, static_cast<socklen_t>(sizeof(on))) == -1) {
    std::string str;
    StringAppendF(&str, "setsockopt SO_REUSEPORT: %s", strerror(errno));
    return Status::IOError(str);
  }
  return Status::OK();
#endif
}

Status SetKeepAlive(int socketfd, bool alive) {
  int on = alive ? 1 : 0;
  if (::setsockopt(socketfd, SOL_SOCKET, SO_KEEPALIVE, 
                   &on, static_cast<socklen_t>(sizeof(on))) == -1) {
    std::string str;
    StringAppendF(&str, "setsockopt SO_KEEPALIVE: %s", strerror(errno));
    return Status::IOError(str);
  } 
  return Status::OK();
}


Status SetTcpNoDelay(int socketfd, bool notdelay) {
  int on = notdelay ? 1 : 0;
  if (::setsockopt(socketfd, IPPROTO_TCP, TCP_NODELAY, 
                   &on, static_cast<socklen_t>(sizeof(on))) == -1) {
    std::string str;
    StringAppendF(&str, "setsockopt TCP_NODELAY: %s", strerror(errno));
    return Status::IOError(str);
  } 
  return Status::OK();
}

Status CheckSocketError(int socketfd) {
  int err = 0;
  socklen_t errlen = static_cast<socklen_t>(sizeof(err));

  if (::getsockopt(socketfd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
    std::string str;
    StringAppendF(&str, "getsockopt (SO_ERROR): %s", strerror(errno));
    return Status::IOError(str);
  }
  if (err) {
    errno = err;
    return Status::IOError(strerror(errno));
  }
  return Status::OK();
}

static Status GenericResolve(const char* hostname, 
                             char* ipbuf, 
                             size_t ipbuf_size, 
                             bool ip_only) {
  struct addrinfo hints, *result;
  int error;

  memset(&hints, 0, sizeof(hints));
  if (ip_only) {
    hints.ai_flags = AI_NUMERICHOST;
  }
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  error = ::getaddrinfo(hostname, NULL, &hints, &result);
  if (error != 0) {
    std::string str;
    StringAppendF(&str, "getaddrinfo: %s", gai_strerror(error));
    return Status::IOError(str);
  }
  if (result->ai_family == AF_INET) {
    assert(ipbuf_size >= INET_ADDRSTRLEN);
    struct sockaddr_in* sa4 = 
        reinterpret_cast<struct sockaddr_in*>(result->ai_addr);
    ::inet_ntop(AF_INET, 
                &(sa4->sin_addr), 
                ipbuf, 
                static_cast<socklen_t>(ipbuf_size));
  } else if (result->ai_family == AF_INET6){
    assert(ipbuf_size >= INET6_ADDRSTRLEN);
    struct sockaddr_in6* sa6 =
        reinterpret_cast<struct sockaddr_in6*>(result->ai_addr);
    ::inet_ntop(AF_INET6, 
                &(sa6->sin6_addr), 
                ipbuf, 
                static_cast<socklen_t>(ipbuf_size));
  }

  ::freeaddrinfo(result);
  return Status::OK();
}

Status Resolve(const char* hostname, char* ipbuf, size_t ipbuf_size) {
  return GenericResolve(hostname, ipbuf, ipbuf_size, false);
}

Status ResolveIP(const char* hostname, char* ipbuf, size_t ipbuf_size) {
  return GenericResolve(hostname, ipbuf, ipbuf_size, true);
}

int FormatAddr(const char* ip, uint16_t port, char* buf, size_t buf_size) {
  return snprintf(buf, buf_size, strchr(ip, ':') ?
                  "[%s]:%d" : "%s:%u", ip, port);
}

int FormatPeer(int socketfd, char* buf, size_t buf_size) {
  struct sockaddr_storage sa(std::move(PeerSockAddr(socketfd)));
  return SockAddrToIPPort(reinterpret_cast<struct sockaddr*>(&sa), 
                          buf, 
                          buf_size);
}

int FormatLocal(int socketfd, char* buf, size_t buf_size) {
  struct  sockaddr_storage sa(std::move(LocalSockAddr(socketfd)));
  return SockAddrToIPPort(reinterpret_cast<struct sockaddr*>(&sa), 
                          buf, 
                          buf_size);
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
    MIRANTS_LOG(ERROR) << "inet_pton failed";
  }
}

void IPPortToSockAddr(const char* ip, 
                      uint16_t port, 
                      struct sockaddr_in6* sa6) {
  sa6->sin6_family = AF_INET6;
  sa6->sin6_port = htons(port);
  if (::inet_pton(AF_INET6, ip, &sa6->sin6_addr) <= 0) {
    MIRANTS_LOG(ERROR) << "inet_pton failed";
  }
}

struct sockaddr_storage PeerSockAddr(int socketfd) {
  struct sockaddr_storage sa;
  socklen_t salen = sizeof(sa);

  if (::getpeername(socketfd, 
                    reinterpret_cast<struct sockaddr*>(&sa), &salen) == -1) {
    MIRANTS_LOG(ERROR) << "getpeername failed";
  }
  return sa;
}

struct sockaddr_storage LocalSockAddr(int socketfd) {
  struct sockaddr_storage sa;
  socklen_t salen = sizeof(sa);
  if (::getsockname(socketfd, 
                    reinterpret_cast<struct sockaddr*>(&sa), &salen) == -1) {
    MIRANTS_LOG(ERROR) << "getsockname failed";
  }
  return sa;
}

int IsSelfConnect(int socketfd) {
  struct sockaddr_storage localaddr = LocalSockAddr(socketfd);
  struct sockaddr_storage peeraddr = PeerSockAddr(socketfd);
  return memcmp(&localaddr, &peeraddr, sizeof(sockaddr_storage));
}

}  // namespace sockets
}  // namespace mirants
