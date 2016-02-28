#include "mirants/core/timer.h"
#include "mirants/core/buffer.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"
#include "mirants/core/tcp_server.h"
#include "mirants/core/tcp_connection.h"
#include "mirants/port/currentthread.h"
#include "mirants/util/stringprintf.h"
#include "mirants/util/logging.h"
#include "mirants/util/slice.h"
#include "mirants/util/timestamp.h"
#include <unistd.h>

using namespace std::placeholders;

namespace mirants {

class TimerServer {
 public:
  TimerServer(EventLoop* ev, const SockAddr& addr)
      : ev_(ev),
        server_(ev, addr, "TimerServer", 4),
        num_(0) {
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

  void ConnMessage() {
    MIRANTS_LOG(TRACE) << "Has " << num_ << " connections were built.";
  }

  void TimerTest() {
    MIRANTS_LOG(TRACE) << "TimerServer::TimerTest - "
                       << " pid=" << getpid()
                       << " tid=" << port::CurrentThread::Tid()
                       << " timestamp=" << Timestamp::Now().FormatTimestamp();
  }

 private:
  void OnConnect(const TcpConnectionPtr& ptr) {
    num_++;
    Slice s("connect successfully!");
    ptr->SendMessage(s);
  }

  void OnMessage(const TcpConnectionPtr& ptr, Buffer* buf) {
    std::string s(buf->RetrieveAllAsString());
    std::string message = 
        StringPrintf("Recieve your message: %s successfully!", s.c_str());
    ptr->SendMessage(std::move(message));
  }

  void OnWriteComplete(const TcpConnectionPtr& ptr) {
    MIRANTS_LOG(INFO) << "TimerServer has wrote completely!";
  }

  EventLoop* ev_;
  TcpServer server_;
  int64_t num_;
};

}  // namespace mirants

int main(int argc, char** argv) {
  mirants::EventLoop ev;
  mirants::SockAddr addr(5666);
  mirants::TimerServer server(&ev, addr);
  server.Start();
  mirants::Timer* t1 = ev.RunEvery(10, 
      std::bind(&mirants::TimerServer::ConnMessage, &server));
  mirants::Timer* t2 = ev.RunAfter(10,
      std::bind(&mirants::TimerServer::TimerTest, &server));
  mirants::Timer* t3 = ev.RunAt(mirants::Timestamp::Now(),
      std::bind(&mirants::TimerServer::TimerTest, &server));
  (void)t1;
  (void)t3;
  // ev.DeleteTimer(t1);
  ev.DeleteTimer(t2);
  ev.Loop();
}
