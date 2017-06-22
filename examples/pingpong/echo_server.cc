// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <inttypes.h>
#include <unistd.h>

#include "voyager/core/callback.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/core/tcp_server.h"
#include "voyager/util/logging.h"
#include "voyager/util/stringprintf.h"

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
};

}  // namespace voyager

int main(int argc, char** argv) {
  printf("pid=%d, tid=%" PRIu64 "\n", getpid(),
         voyager::port::CurrentThread::Tid());
  voyager::EventLoop ev;
  voyager::SockAddr addr(5666);
  voyager::EchoServer server(&ev, addr);
  server.Start();
  ev.Loop();
  return 0;
}
