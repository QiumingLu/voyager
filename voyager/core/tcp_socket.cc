#include "voyager/core/tcp_socket.h"

#include "voyager/core/socket_util.h"
#include "voyager/util/status.h"
#include "voyager/util/logging.h"

namespace voyager {

TcpSocket::~TcpSocket() {
  sockets::CloseFd(socketfd_);
}

void TcpSocket::BindAddress(const struct sockaddr* sa, socklen_t salen) {
  sockets::BindAddress(socketfd_, sa, salen);
}

void TcpSocket::Listen(int backlog) {
  sockets::Listen(socketfd_, backlog);
}

int TcpSocket::Accept(struct sockaddr* peer_sa, socklen_t* salen) {
  int fd = sockets::Accept(socketfd_, peer_sa, salen);
  return fd;
}

void TcpSocket::SetReuseAddr(bool on) {
  Status st = sockets::SetReuseAddr(socketfd_, on);
  if (!st.ok()) {
    VOYAGER_LOG(ERROR) << st;
  }
}

void TcpSocket::SetReusePort(bool on) {
  Status st = sockets::SetReusePort(socketfd_, on);
  if (!st.ok()) {
    VOYAGER_LOG(ERROR) << st;
  }
}

void TcpSocket::SetKeepAlive(bool on) {
  Status st = sockets::SetKeepAlive(socketfd_, on);
  if (!st.ok()) {
    VOYAGER_LOG(ERROR) << st;
  }
}

void TcpSocket::SetTcpNoDelay(bool on) {
  Status st = sockets::SetTcpNoDelay(socketfd_, on);
  if (!st.ok()) {
    VOYAGER_LOG(ERROR) << st;
  }
}

void TcpSocket::ShutDownWrite() {
  sockets::ShutDownWrite(socketfd_);
}

}  // namespace voyager
