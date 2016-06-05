#include "examples/http/httpserver.h"
#include "mirants/core/eventloop.h"

int main() {
  mirants::EventLoop ev;
  mirants::SockAddr addr(5666);
  http::HttpServer server(&ev, addr);
  server.Start();
  ev.Loop();
  return 0;
}
