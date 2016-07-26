#include <voyager/core/tcp_server.h>
#include <voyager/core/eventloop.h>
#include <voyager/core/sockaddr.h>
#include <voyager/core/tcp_connection.h>
#include <voyager/core/callback.h>
#include <voyager/util/logging.h>
#include <unistd.h>
#include <inttypes.h>
#include <gperftools/profiler.h>

using namespace std::placeholders;

namespace voyager {

class EchoServer {
 public:
  EchoServer(EventLoop* ev, const SockAddr& addr)
      : server_(ev, addr, "EchoServer", 4) {
    server_.SetConnectionCallback(
		std::bind(&EchoServer::Connect, this, _1));
    server_.SetMessageCallback(
		std::bind(&EchoServer::Message, this, _1, _2));
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
    conn_ptr->SendMessage(buf);
    conn_ptr->ShutDown();
  }

  TcpServer server_;

  // No copying allow
  EchoServer(const EchoServer&);
  void operator=(const EchoServer&);
};

}  // namespace voyager

// 加入了Google PerfTools来测试,如果不需要可以去掉
int main(int argc, char** argv) {
  ProfilerStart("MyProfile");
  voyager::SetLogHandler(NULL);
  printf("pid=%d, tid=%" PRIu64"\n",
         getpid(), voyager::port::CurrentThread::Tid());
  voyager::EventLoop ev;
  voyager::SockAddr addr(5666);
  voyager::EchoServer server(&ev, addr);
  ev.RunAfter(180, std::bind(&voyager::EventLoop::Exit, &ev));
  server.Start();
  ev.Loop();
  ProfilerStop();
  return 0;
}
