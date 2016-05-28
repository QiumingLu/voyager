#include "mirants/core/tcp_server.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"
#include "mirants/core/tcp_connection.h"
#include "mirants/core/callback.h"
#include "mirants/util/stringprintf.h"
#include "mirants/util/logging.h"
#include <unistd.h>

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
    if (conn_ptr->IsConnected()) {
      conn_ptr->SetTcpNoDelay(true);
    }
  }

  void Message(const TcpConnectionPtr& conn_ptr, Buffer* buf) {
    std::string s = buf->RetrieveAllAsString();
    conn_ptr->SendMessage(std::move(s));
  }

  EventLoop* ev_;
  TcpServer server_;
};

}  // namespace mirants

int main(int argc, char** argv) {
  printf("pid=%d, tid=%d\n", getpid(), mirants::port::CurrentThread::Tid());
  mirants::EventLoop ev;
  mirants::SockAddr addr(5666);
  mirants::EchoServer server(&ev, addr);
  server.Start();
  ev.Loop();
  return 0;
}
