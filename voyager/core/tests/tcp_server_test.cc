// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/tcp_server.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"

using namespace voyager;

TcpServer* g_server = nullptr;

void DeleteServer() {
  if (g_server) {
    delete g_server;
  }
}

int main(int argc, char** argv) {
  EventLoop eventloop;
  SockAddr addr(5666);
  g_server = new TcpServer(&eventloop, addr, "Voyager", 4);
  g_server->Start();
  eventloop.RunAfter(5000, []() { DeleteServer(); });
  eventloop.RunAfter(6000, [&eventloop]() { eventloop.Exit(); });
  eventloop.Loop();
  return 0;
}
