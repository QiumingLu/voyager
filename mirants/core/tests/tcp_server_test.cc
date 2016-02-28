#include "mirants/core/tcp_server.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"

using namespace mirants;

TcpServer* g_server = NULL;

void DeleteServer() {
  if (g_server) {
    delete g_server;
  }
}

int main(int argc, char** argv) {
  EventLoop eventloop;
  SockAddr addr(5666);
  g_server = new TcpServer(&eventloop, addr, "Mirants", 4);
  g_server->Start();
  eventloop.RunAfter(20, std::bind(DeleteServer));
  eventloop.Loop();
  return 0;
}
