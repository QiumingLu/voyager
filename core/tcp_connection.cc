#include "core/tcp_connection.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"

namespace mirants {

TcpConnection::TcpConnection(const std::string& name, EventLoop* ev, int fd,
                             const SockAddr& local_addr,
                             struct sockaddr_storage* peer_sa)
    : name_(name), eventloop_(ev)
{}
 
TcpConnection::~TcpConnection() {
}

void TcpConnection::EstablishConnection() {

}

void TcpConnection::DeleteConnection() {
  
}

}  // namespace mirants
