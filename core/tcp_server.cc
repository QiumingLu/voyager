#include "tcp_server.h"
#include "core/acceptor.h"
#include "sockaddr.h"

namespace mirants {

TcpServer::TcpServer(const SockAddr& addr, int backlog) 
    : servinfo_(addr.AddInfo()),
      acceptor_ptr_(new Acceptor(servinfo_, backlog)) {
}

}  // namespace mirants
