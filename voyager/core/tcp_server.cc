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

TcpServer::TcpServer(EventLoop* ev,
                     const SockAddr& addr,
                     const std::string& name,
                     int thread_size,
                     int backlog,
                     bool reuseport)
    : eventloop_(CHECK_NOTNULL(ev)),
      ipbuf_(addr.Ipbuf()),
      name_(name),
      schedule_(new Schedule(eventloop_, thread_size-1)),
      acceptor_(new TcpAcceptor(eventloop_, addr, backlog, reuseport)) {
  acceptor_->SetNewConnectionCallback(
      std::bind(&TcpServer::NewConnection, this,
                std::placeholders::_1, std::placeholders::_2));
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

void TcpServer::NewConnection(int fd, const struct sockaddr_storage& sa) {
  eventloop_->AssertInMyLoop();

  char peer[64];
  char conn_name[256];
  SockAddr::FormatAddress(reinterpret_cast<const sockaddr*>(&sa),
                          peer, sizeof(peer));
  snprintf(conn_name, sizeof(conn_name),
           "%s-%s#%d", ipbuf_.c_str(), peer, conn_id_.GetNext());

  VOYAGER_LOG(INFO) << "TcpServer::NewConnection [" << name_
                    << "] - new connection [" << conn_name
                    << "] from " << peer;

  EventLoop* ev = schedule_->AssignLoop();
  TcpConnectionPtr conn_ptr(new TcpConnection(conn_name, ev, fd));

  conn_ptr->SetConnectionCallback(connection_cb_);
  conn_ptr->SetCloseCallback(close_cb_);
  conn_ptr->SetWriteCompleteCallback(writecomplete_cb_);
  conn_ptr->SetMessageCallback(message_cb_);

  ev->RunInLoop([conn_ptr]() {
    conn_ptr->StartWorking();
  });
}

}  // namespace voyager
