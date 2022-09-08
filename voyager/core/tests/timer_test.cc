// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/buffer.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/core/tcp_server.h"
#include "voyager/core/timerlist.h"
#include "voyager/util/logging.h"
#include "voyager/util/timeops.h"

#include <sys/types.h>
#include <unistd.h>

using namespace std::placeholders;

namespace voyager {

class TimerServer {
 public:
  TimerServer(EventLoop* ev, const SockAddr& addr)
      : server_(ev, addr, "TimerServer", 4), ev_(ev) {
    server_.SetConnectionCallback(std::bind(&TimerServer::OnConnect, this, _1));
    server_.SetMessageCallback(
        std::bind(&TimerServer::OnMessage, this, _1, _2));
    server_.SetWriteCompleteCallback(
        std::bind(&TimerServer::OnWriteComplete, this, _1));
  }

  void Start() { server_.Start(); }

  void TimerTest() {
    VOYAGER_LOG(INFO) << "TimerServer::TimerTest - "
                      << " pid=" << getpid()
                      << " tid=" << std::this_thread::get_id()
                      << " timestamp=" << timeops::NowMicros();
    ev_->Exit();
  }

 private:
  void OnConnect(const TcpConnectionPtr& ptr) {
    ptr->SendMessage(std::string("connect successfully!"));
  }

  void OnMessage(const TcpConnectionPtr& ptr, Buffer* buf) {
    ptr->SendMessage(std::string("Recieve your message successfully!"));
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
  ev.RunAfter(10000, [&server]() { server.TimerTest(); });
  ev.Loop();
}
