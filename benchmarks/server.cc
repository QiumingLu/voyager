#include "mirants/core/tcp_server.h"
#include "mirants/core/callback.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"
#include "mirants/core/tcp_connection.h"
#include <iostream>

class Server {
 public:
  Server(mirants::EventLoop* ev, 
         const mirants::SockAddr& addr,
         const std::string& name,
         int thread_count)
      : server_(ev, addr, name, thread_count) {
    using namespace std::placeholders;
    server_.SetConnectionCallback(
        std::bind(&Server::ConnectCallback, this, _1));
    server_.SetMessageCallback(
        std::bind(&Server::MessageCallback, this, _1, _2));
  }

  void Start() {
    server_.Start();
  }

 private:
  void ConnectCallback(const mirants::TcpConnectionPtr& ptr) {
    ptr->SetTcpNoDelay(true);
  }
  
  void MessageCallback(const mirants::TcpConnectionPtr& ptr,
                       mirants::Buffer* buf) {
    size_t size = buf->ReadableSize();
    mirants::Slice s(buf->Peek(), size);
    ptr->SendMessage(s);
    buf->Retrieve(size);
  }

  mirants::TcpServer server_;
};

int main(int argc, char** argv) {
  try {
    if (argc != 4) {
      std::cerr << "Usage: server <host> <port> <threads>\n";
      return 1;
    }
    const char* host = argv[1];
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    int thread_count = atoi(argv[3]);

    mirants::SockAddr addr(host, port);
    mirants::EventLoop ev;
    Server server(&ev, addr, "server", thread_count);
    
    server.Start();
    ev.Loop();

  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
