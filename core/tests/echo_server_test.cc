#include "core/tcp_server.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"
#include "core/tcp_connection.h"
#include "core/callback.h"
#include <stdio.h>

using namespace std::placeholders;

namespace mirants {

class EchoServer {
 public:
  EchoServer(EventLoop* ev, const SockAddr& addr)
      : ev_(ev),
        server_(ev, addr, "EchoServer", 4) {
    server_.SetConnectionCallback(std::bind(&EchoServer::Connect, this, _1));
    server_.SetMessageCallback(std::bind(&EchoServer::Message, this, _1, _2));
  }

  void Start() {
    server_.Start();
  }

 private:
  void Connect(const TcpConnectionPtr& conn_ptr) {
    printf("Connection %s has been built\n", conn_ptr->name().c_str());
  }

  void Message(const TcpConnectionPtr& conn_ptr, Buffer* buf) {
    printf("Connection Name: %s ", conn_ptr->name().c_str());
    printf("recieve %zd bytes\n", buf->ReadableSize());
  }

  EventLoop* ev_;
  TcpServer server_;
};

}  // namespace mirants

int main(int argc, char** argv) {
  mirants::EventLoop ev;
  mirants::SockAddr addr(5666);
  mirants::EchoServer server(&ev, addr);
  server.Start();
  ev.Loop();
  return 0;
}
