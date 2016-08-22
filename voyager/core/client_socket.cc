#include "voyager/core/client_socket.h"

namespace voyager {

int ClientSocket::Connect(const struct sockaddr* sa,
                          socklen_t salen) const {
  return ::connect(fd_, sa, salen);
}


}  // namespace voyager
