// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <voyager/core/callback.h>
#include <voyager/core/eventloop.h>
#include <voyager/core/sockaddr.h>
#include <voyager/core/tcp_connection.h>
#include <voyager/core/tcp_server.h>
#include <voyager/util/logging.h>
#include <voyager/util/timeops.h>
#ifdef __linux__
#include <voyager/core/newtimer.h>
#endif

// #include <gperftools/profiler.h>

namespace voyager {

class EchoServer {
 public:
  EchoServer(EventLoop* ev, const SockAddr& addr)
      : server_(ev, addr, "EchoServer", 4) {
    server_.SetConnectionCallback(
        std::bind(&EchoServer::Connect, this, std::placeholders::_1));
    server_.SetMessageCallback(std::bind(&EchoServer::Message, this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
  }

  void Start() { server_.Start(); }

 private:
  void Connect(const TcpConnectionPtr& conn_ptr) {}

  void Message(const TcpConnectionPtr& conn_ptr, Buffer* buf) {
    conn_ptr->SendMessage(buf);
  }

  TcpServer server_;

  // No copying allowed
  EchoServer(const EchoServer&);
  void operator=(const EchoServer&);
};

}  // namespace voyager

// 加入了Google PerfTools来测试,如果不需要可以去掉
int main(int argc, char** argv) {
  // ProfilerStart("MyProfile");
  voyager::SetLogHandler(nullptr);
  voyager::EventLoop ev;
  voyager::SockAddr addr("127.0.0.1", 5666);
  voyager::EchoServer server(&ev, addr);
  ev.RunAfter(180 * 1000, [&ev]() { ev.Exit(); });
  server.Start();
  ev.Loop();
  // ProfilerStop();
  return 0;
}
