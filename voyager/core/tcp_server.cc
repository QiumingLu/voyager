// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/tcp_server.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/schedule.h"
#include "voyager/core/tcp_acceptor.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/util/logging.h"

namespace voyager {

port::SequenceNumber TcpServer::conn_id_;

TcpServer::TcpServer(EventLoop* ev, const SockAddr& addr,
                     const std::string& name, int thread_size, int backlog,
                     bool reuseport)
    : eventloop_(CHECK_NOTNULL(ev)),
      addr_(addr),
      name_(name),
      schedule_(new Schedule(eventloop_, thread_size - 1)),
      acceptor_(new TcpAcceptor(eventloop_, addr, backlog, reuseport)) {
  acceptor_->SetNewConnectionCallback(std::bind(&TcpServer::NewConnection, this,
                                                std::placeholders::_1,
                                                std::placeholders::_2));
  VOYAGER_LOG(INFO) << "TcpServer::TcpServer [" << name_ << "] is running";
}

TcpServer::~TcpServer() {
  VOYAGER_LOG(INFO) << "TcpServer::~TcpServer [" << name_ << "] is down";
}

void TcpServer::Start() {
  eventloop_->RunInLoop([this]() {
    if (seq_.GetNext() == 0) {
      schedule_->Start();
      assert(!acceptor_->IsListenning());
      acceptor_->EnableListen();
    }
  });
}

const std::vector<EventLoop*>* TcpServer::AllLoops() const {
  return schedule_->AllLoops();
}

void TcpServer::NewConnection(int fd, const struct sockaddr_storage& sa) {
  eventloop_->AssertInMyLoop();

  SockAddr peer(sa);
  char conn_name[256];
  snprintf(conn_name, sizeof(conn_name), "%s-%s#%d", addr_.Ipbuf().c_str(),
           peer.Ipbuf().c_str(), conn_id_.GetNext());

  VOYAGER_LOG(INFO) << "TcpServer::NewConnection [" << name_
                    << "] - new connection [" << conn_name << "] from "
                    << peer.Ipbuf();

  EventLoop* ev = schedule_->AssignLoop();
  TcpConnectionPtr ptr(new TcpConnection(conn_name, ev, fd, addr_, peer));

  ptr->SetConnectionCallback(connection_cb_);
  ptr->SetCloseCallback(close_cb_);
  ptr->SetWriteCompleteCallback(writecomplete_cb_);
  ptr->SetMessageCallback(message_cb_);

  ev->RunInLoop([ptr]() { ptr->StartWorking(); });
}

}  // namespace voyager
