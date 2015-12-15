#include "tcp_socket.h"
#include "core/socket_util.h"
#include "util/status.h"
#include "util/logging.h"

namespace mirants {

TcpSocket::~TcpSocket() {
  SocketsUtil::CloseFd(socketfd_);
}

void TcpSocket::BindAddress(const struct sockaddr_in* local_sa4) {
  sockets::BindAddress(socketfd_, 
                       reinterpret_cast<const struct sockaddr*>(local_sa4),
                       static_cast<socklen_t>(sizeof(*local_sa4)));
}

void TcpSocket::BindAddress(const struct sockaddr_in6* local_sa6) {
  sockets::BindAddress(socketfd_,
                       reinterpret_cast<const struct sockaddr*>(local_sa6),
                       static_cast<socklen_t>(sizeof(*local_sa6)));
}

void TcpSocket::Listen() {
  sockets::Listen(socketfd_);
}

int Accept(struct sockaddr_in* peer_sa4) {
  int fd = sockets::Accept(socketfd_,
                           reinterpret_cast<struct sockaddr*>(peer_sa4),
                           static_cast<socklen_t>(sizeof(*peer_sa4)));
  return fd;
}

int Accept(struct sockaddr_in6* peer_sa6) {
  int fd = sockets::Accept(socketfd_,
                           reinterpret_cast<struct sockaddr*>(peer_sa6),
                           static_cast<socklen_t>(sizeof(*peer_sa6)));
  return fd;
}

void TcpSocket::SetReuseAddr(bool on) {
  Status st = sockets::SetReuseAddr(socketfd_, on);
  if (!st.ok()) {
    MIRANTS_LOG(ERROR) << st;
  }
}

void TcpSocket::SetReusePort(bool on) {
  Status st = sockets::SetReusePort(socketfd_, on);
  if (st.ok()) {
    MIRANTS_LOG(ERROR) << st;
  }
}

void TcpSocket::SetKeepAlive(bool on) {
  Status st = sockets::SetKeepAlive(socketfd_, on);
  if (st.ok()) {
    MIRANTS_LOG(ERROR) << st;
  }
}

void TcpSocket::SetTcpNoDelay(bool on) {
  Status st = sockets::SetTcpNoDelay(socketfd_, on);
  if (st.ok()) {
    MIRANTS_LOG(ERROR) << st;
  }
}

}  // namespace mirants
