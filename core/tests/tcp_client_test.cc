#include "core/tcp_cleint.h"
#include "core/sockaddr.h"

namespace mirants {
}  // namespace mirants

int main(int argc, char** argv) {
  SockAddr serveraddr("127.0.0.1", 5666);
  TcpClient client(serveraddr);
  client.TcpConnect();
  return 0;
}
