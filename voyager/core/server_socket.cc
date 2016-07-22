#include "voyager/core/server_socket.h"
#include "voyager/util/logging.h"

namespace voyager {
  
void ServerSocket::Bind(const struct sockaddr* sa, socklen_t salen) {
  if (::bind(fd_, sa, salen) == -1) {
    VOYAGER_LOG(FATAL) << "bind: " << strerror(errno);
  }
}

void ServerSocket::Listen(int backlog) {
  if (::listen(fd_, backlog) == -1) {
    VOYAGER_LOG(FATAL) << "listen: " << strerror(errno);
  }
}

int ServerSocket::Accept(struct sockaddr* sa, socklen_t* salen) {
  int connectfd = ::accept(fd_, sa, salen);
  if (connectfd == -1) {
    int err = errno;
    switch (err) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO: 
      case EPERM:
      case EMFILE: 
        VOYAGER_LOG(ERROR) << "accept: " << strerror(err);
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        VOYAGER_LOG(FATAL) << "accept: " << strerror(err);
        break;
      default:
        VOYAGER_LOG(FATAL) << "accept: " << strerror(err);      
    }
  }
  return connectfd;
}

}  // namespace voyager
