// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/tcp_client.h"
#include "voyager/core/tcp_connector.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/util/logging.h"

namespace voyager {

port::SequenceNumber TcpClient::conn_id_;

TcpClient::TcpClient(EventLoop* ev,
                     const SockAddr& addr,
                     const std::string& name)
    : ev_(CHECK_NOTNULL(ev)),
      addr_(addr),
      name_(name),
      connector_ptr_(new TcpConnector(ev, addr)),
      connect_(false) {
  connector_ptr_->SetNewConnectionCallback(
      std::bind(&TcpClient::NewConnection, this, std::placeholders::_1));
  VOYAGER_LOG(INFO) << "TcpClient::TcpClient [" << name_ << "] is running";
}

TcpClient::~TcpClient() {
  VOYAGER_LOG(INFO) << "TcpClient::~TcpClient [" << name_ << "] is down";
}

void TcpClient::Connect(bool retry) {
  // 表示已经经过连接建立和连接断开的过程
  bool expected = true;

  if (!weak_ptr_.lock()) {
    connect_.compare_exchange_weak(expected, false);
  }

  if (!connect_) {
    VOYAGER_LOG(INFO) << "TcpClient::Connect [" << name_
                     << "] - connecting to " << addr_.Ipbuf();
    connector_ptr_->Start(retry);
    connect_ = true;
  }
}

void TcpClient::Close() {
  // （1) 连接未成功
  // （2）连接成功但未断开
  if (connect_) {
    connector_ptr_->Stop();
    TcpConnectionPtr ptr = weak_ptr_.lock();
    if (ptr) {
      ptr->ShutDown();
    }
    connect_ = false;
  }
}

void TcpClient::NewConnection(int fd) {
  ev_->AssertInMyLoop();

  SockAddr local(SockAddr::LocalSockAddr(fd));
  char conn_name[256];
  snprintf(conn_name, sizeof(conn_name), "%s-%s#%d",
           addr_.Ipbuf().c_str(), local.Ipbuf().c_str(), conn_id_.GetNext());

  VOYAGER_LOG(INFO) << "TcpClient::NewConnection[" << name_
                    << "] - new connection[" << conn_name
                    << "] to " << addr_.Ipbuf();

  TcpConnectionPtr ptr(new TcpConnection(conn_name, ev_, fd, local, addr_));
  ptr->SetConnectionCallback(connection_cb_);
  ptr->SetMessageCallback(message_cb_);
  ptr->SetWriteCompleteCallback(writecomplete_cb_);
  ptr->SetCloseCallback(close_cb_);
  ptr->StartWorking();
  weak_ptr_ = ptr;
}

void TcpClient::SetConnectFailureCallback(const ConnectFailureCallback& cb) {
  connector_ptr_->SetConnectFailureCallback(cb);
}

void TcpClient::SetConnectFailureCallback(ConnectFailureCallback&& cb) {
  connector_ptr_->SetConnectFailureCallback(std::move(cb));
}

}  // namespace voyager
