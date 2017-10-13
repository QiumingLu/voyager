// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_TCP_SERVER_H_
#define VOYAGER_CORE_TCP_SERVER_H_

#include <netdb.h>

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "voyager/core/callback.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"

namespace voyager {

class TcpAcceptor;
class Schedule;

class TcpServer {
 public:
  TcpServer(EventLoop* ev, const SockAddr& addr,
            const std::string& name = std::string("VoyagerServer"),
            int thread_size = 0, int backlog = SOMAXCONN,
            bool reuseport = false);
  ~TcpServer();

  const std::string& name() const { return name_; }

  void SetConnectionCallback(const ConnectionCallback& cb) {
    connection_cb_ = cb;
  }
  void SetCloseCallback(const CloseCallback& cb) { close_cb_ = cb; }
  void SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writecomplete_cb_ = cb;
  }
  void SetMessageCallback(const MessageCallback& cb) { message_cb_ = cb; }

  void SetConnectionCallback(ConnectionCallback&& cb) {
    connection_cb_ = std::move(cb);
  }
  void SetCloseCallback(CloseCallback&& cb) { close_cb_ = std::move(cb); }
  void SetWriteCompleteCallback(WriteCompleteCallback&& cb) {
    writecomplete_cb_ = std::move(cb);
  }
  void SetMessageCallback(MessageCallback&& cb) { message_cb_ = std::move(cb); }

  void Start();

  // All loops for schedule tcp connections.
  const std::vector<EventLoop*>* AllLoops() const;

 private:
  void NewConnection(int fd, const struct sockaddr_storage& sa);

  static std::atomic<int> conn_id_;

  EventLoop* eventloop_;
  SockAddr addr_;
  std::string name_;
  std::atomic<bool> started_;

  ConnectionCallback connection_cb_;
  CloseCallback close_cb_;
  WriteCompleteCallback writecomplete_cb_;
  MessageCallback message_cb_;

  std::unique_ptr<Schedule> schedule_;
  std::unique_ptr<TcpAcceptor> acceptor_;

  // No copying allowed
  TcpServer(const TcpServer&);
  void operator=(const TcpServer&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_SERVER_H_
