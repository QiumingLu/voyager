#include "mirants/core/tcp_server.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"
#include "mirants/port/thread.h"

using namespace mirants;
using namespace mirants::port;

TcpServer *g_server;

void ThreadFunc() {
  g_server->Start();
}

int main(int argc, char** argv) {
  SockAddr addr("127.0.0.1", 5666);
  std::string server_name("MIRANTSServer");
  EventLoop loop;
  TcpServer server(&loop, addr, server_name);
  g_server = &server;
  Thread t(ThreadFunc);
  t.Start();
  loop.Loop();
  return 0;
}
