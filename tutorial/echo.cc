#include <voyager/core/tcp_server.h>
#include <voyager/core/eventloop.h>
#include <voyager/core/sockaddr.h>
#include <voyager/core/tcp_connection.h>
#include <voyager/core/callback.h>
#include <voyager/util/logging.h>
#include <voyager/util/timeops.h>
#include <unistd.h>
#include <inttypes.h>
#include <gperftools/profiler.h>
#ifdef __linux__
#include <voyager/core/newtimer.h>
#endif

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
  voyager::NewTimer timer(&ev, std::bind(&voyager::EventLoop::Exit, &ev));
#ifdef __linux__
  timer.SetTime(180 * (voyager::timeops::kNonasSecondsPerSecond), 0);
#else
  ev.RunAfter(std::bind(&voyager::EventLoop::Exit, &ev), 180*1000000);
#endif
  server.Start();
  ev.Loop();
  ProfilerStop();
  return 0;
}
