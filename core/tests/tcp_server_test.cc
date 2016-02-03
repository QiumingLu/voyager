#include "core/tcp_server.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"

namespace mirants {
}  // namespace mirants

int main(int argc, char** argv) {
  mirants::EventLoop eventloop;
  mirants::SockAddr addr("127.0.0.1", 5666);
  std::string server_name("MirantsServer");
  mirants::TcpServer tcpserver(&eventloop, addr, server_name);
  tcpserver.Start();
  eventloop.Loop();
  return 0;
}
