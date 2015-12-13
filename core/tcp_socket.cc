#include "tcp_socket.h"

#include <unistd.h>
#include "core/socket_util.h"

namespace mirants {

TcpSocket::~TcpSocket() {
  SocketsUtil::CloseFd(sockfd_);
}

void TcpSocket::BindAddress(const struct sockadd* addr) {
}

}  // namespace mirants
