#include <unistd.h>
#include <inttypes.h>
#include <voyager/core/tcp_server.h>
#include <voyager/core/eventloop.h>
#include <voyager/core/sockaddr.h>
#include <voyager/core/tcp_connection.h>
#include <voyager/core/callback.h>
#include <voyager/util/logging.h>
#include <voyager/util/timeops.h>
//#include <gperftools/profiler.h>
#ifdef __linux__
#include <voyager/core/newtimer.h>
#endif

namespace voyager {

class EchoServer {
 public:
  EchoServer(EventLoop* ev, const SockAddr& addr)
      : server_(ev, addr, "EchoServer", 4) {
    server_.SetConnectionCallback(std::bind(&EchoServer::Connect,
                                            this,
                                            std::placeholders::_1));
    server_.SetMessageCallback(std::bind(&EchoServer::Message,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
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

  // No copying allowed
  EchoServer(const EchoServer&);
  void operator=(const EchoServer&);
};

}  // namespace voyager

// 加入了Google PerfTools来测试,如果不需要可以去掉
int main(int argc, char** argv) {
//  ProfilerStart("MyProfile");
  voyager::SetLogHandler(nullptr);
  printf("pid=%d, tid=%" PRIu64"\n",
         getpid(), voyager::port::CurrentThread::Tid());
  voyager::EventLoop ev;
  voyager::SockAddr addr("127.0.0.1", 5666);
  voyager::EchoServer server(&ev, addr);
#ifdef __linux__
  voyager::NewTimer timer(&ev, [&ev]() { ev.Exit(); });
  timer.SetTime(180 * (voyager::timeops::kNonasSecondsPerSecond), 0);
#else
  ev.RunAfter(180*1000000, [&ev]() { ev.Exit(); });
#endif
  server.Start();
  ev.Loop();
//  ProfilerStop();
  return 0;
}
