#include "core/tcp_server.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"

namespace mirants {
}  // namespace mirants

int main(int argc, char** argv) {
  mirants::EventLoop eventloop;
  mirants::SockAddr addr("192.168.1.114", 5666, false);
  mirants::TcpServer tcpserver(&eventloop, addr);
  tcpserver.Start();
  eventloop.Loop();
  return 0;
}
