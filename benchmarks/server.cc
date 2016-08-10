#include "voyager/core/tcp_server.h"
#include "voyager/core/callback.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_connection.h"
#include <iostream>

class Server {
 public:
  Server(voyager::EventLoop* ev, 
         const voyager::SockAddr& addr,
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
  void ConnectCallback(const voyager::TcpConnectionPtr& ptr) {
  }
  
  void MessageCallback(const voyager::TcpConnectionPtr& ptr,
                       voyager::Buffer* buf) {
    size_t size = buf->ReadableSize();
    voyager::Slice s(buf->Peek(), size);
    ptr->SendMessage(s);
    buf->Retrieve(size);
  }

  voyager::TcpServer server_;
};

int main(int argc, char** argv) {
  if (argc != 4) {
    std::cerr << "Usage: server <host> <port> <threads>\n";
    return 1;
  }
  const char* host = argv[1];
  uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
  int thread_count = atoi(argv[3]);

  voyager::SockAddr addr(host, port);
  voyager::EventLoop ev;
  Server server(&ev, addr, "server", thread_count);
    
  server.Start();
  ev.Loop();

  return 0;
}
