#include "core/tcp_client.h"
#include "core/sockaddr.h"

namespace mirants {
}  // namespace mirants

int main(int argc, char** argv) {
  mirants::SockAddr serveraddr("127.0.0.1", 5666);
  mirants::TcpClient client(serveraddr);
  client.TcpConnect();
  return 0;
}
