// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/callback.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/core/tcp_server.h"
#include "voyager/util/logging.h"

#include <inttypes.h>
#include <unistd.h>

using namespace std::placeholders;

namespace voyager {

class EchoServer {
 public:
  EchoServer(EventLoop* ev, const SockAddr& addr)
      : server_(ev, addr, "EchoServer", 4) {
    server_.SetConnectionCallback(std::bind(&EchoServer::Connect, this, _1));
    server_.SetMessageCallback(std::bind(&EchoServer::Message, this, _1, _2));
  }

  void Start() { server_.Start(); }

 private:
  void Connect(const TcpConnectionPtr& conn_ptr) {
    conn_ptr->SendMessage(std::string("Connection has been built!"));
    std::string s1 = "";
    Slice s2("");
    conn_ptr->SendMessage(s1);
    conn_ptr->SendMessage(s2);
  }

  void Message(const TcpConnectionPtr& conn_ptr, Buffer* buf) {
    std::string s = buf->RetrieveAllAsString();
    VOYAGER_LOG(INFO) << s;
    if (s == "That's OK! I close!") {
      Slice message = "Bye!";
      conn_ptr->SendMessage(message);
    } else {
      Slice message = "Nice!";
      conn_ptr->SendMessage(message);
    }
  }

  TcpServer server_;
};

}  // namespace voyager

int main(int argc, char** argv) {
  voyager::EventLoop ev;
  // test for IPV6
  voyager::SockAddr addr("2001:da8:205:40b2:6d12:4a15:b65d:63e3", 5666);
  voyager::EchoServer server(&ev, addr);
  server.Start();
  ev.Loop();
  return 0;
}
