// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_server.h"
#include "voyager/port/thread.h"

using namespace voyager;
using namespace voyager::port;

TcpServer* g_server;

void ThreadFunc() { g_server->Start(); }

int main(int argc, char** argv) {
  SockAddr addr("127.0.0.1", 5666);
  std::string server_name("VOYAGERServer");
  EventLoop loop;
  TcpServer server(&loop, addr, server_name);
  g_server = &server;
  Thread t(ThreadFunc);
  t.Start();
  loop.Loop();
  return 0;
}
