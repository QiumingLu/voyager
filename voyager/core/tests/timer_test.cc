#include "voyager/core/timerlist.h"
#include "voyager/core/buffer.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_server.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/port/currentthread.h"
#include "voyager/util/stringprintf.h"
#include "voyager/util/logging.h"
#include "voyager/util/timeops.h"

#include <sys/types.h>
#include <unistd.h>

using namespace std::placeholders;

namespace voyager {

class TimerServer {
 public:
  TimerServer(EventLoop* ev, const SockAddr& addr)
      : server_(ev, addr, "TimerServer", 4),
        ev_(ev) {
    server_.SetConnectionCallback(
        std::bind(&TimerServer::OnConnect, this, _1));
    server_.SetMessageCallback(
        std::bind(&TimerServer::OnMessage, this, _1, _2));
    server_.SetWriteCompleteCallback(
        std::bind(&TimerServer::OnWriteComplete, this, _1));
  }

  void Start() {
    server_.Start();
  }

  void TimerTest() {
    VOYAGER_LOG(INFO) << "TimerServer::TimerTest - "
                       << " pid=" << getpid()
                       << " tid=" << port::CurrentThread::Tid()
                       << " timestamp=" << timeops::NowMicros();
    ev_->Exit();
  }

 private:
  void OnConnect(const TcpConnectionPtr& ptr) {
    ptr->SendMessage(Slice("connect successfully!"));
  }

  void OnMessage(const TcpConnectionPtr& ptr, Buffer* buf) {
    std::string s(buf->RetrieveAllAsString());
    std::string message = 
        StringPrintf("Recieve your message: %s successfully!", s.c_str());
    ptr->SendMessage(std::move(message));
  }

  void OnWriteComplete(const TcpConnectionPtr& ptr) {
    VOYAGER_LOG(INFO) << "TimerServer has wrote completely!";
  }

  TcpServer server_;
  EventLoop* ev_;
};

}  // namespace voyager


int main(int argc, char** argv) {
  voyager::EventLoop ev;
  voyager::SockAddr addr(5666);
  voyager::TimerServer server(&ev, addr);
  server.Start();
  voyager::TimerList::Timer* t2 = 
      ev.RunAfter(10000000, [&server]() {server.TimerTest(); });
  (void)t2;
  ev.Loop();
}
