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
    std::string  message = StringPrintf("Connection %s has been built\n",
                                        conn_ptr->name().c_str());
    conn_ptr->SendMessage(message);
    std::string s1 = "";
    Slice s2("");
    conn_ptr->SendMessage(s1);
    conn_ptr->SendMessage(s2);
  }

  void Message(const TcpConnectionPtr& conn_ptr, Buffer* buf) {
    std::string s = buf->RetrieveAllAsString();
    MIRANTS_LOG(INFO) << s;
    if (s == "That's OK! I close!") {
      Slice message = "Bye!";
      conn_ptr->SendMessage(message);
    } else {
      Slice message = "Nice!";
      conn_ptr->SendMessage(message);
    }
  }

  EventLoop* ev_;
  TcpServer server_;
};

}  // namespace mirants

int main(int argc, char** argv) {
  printf("pid=%d, tid=%d\n", getpid(), mirants::port::CurrentThread::Tid());
  mirants::EventLoop ev;
  mirants::SockAddr addr("2001:da8:205:40b2:6d12:4a15:b65d:63e3", 5666);
  mirants::EchoServer server(&ev, addr);
  server.Start();
  ev.Loop();
  return 0;
}
