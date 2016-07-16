#include "examples/http/httpserver.h"
#include "voyager/core/eventloop.h"

int main() {
  voyager::EventLoop ev;
  voyager::SockAddr addr(5666);
  http::HttpServer server(&ev, addr, "webserver", 4);
  server.Start();
  // ev.RunAfter(100, std::bind(&voyager::EventLoop::Exit, &ev));
  ev.Loop();
  return 0;
}
