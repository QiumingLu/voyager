// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/tcp_monitor.h"
#include "voyager/util/logging.h"

namespace voyager {

TcpMonitor::TcpMonitor(int max_all_connections, int max_ip_connections)
    : kMaxAllConnections(max_all_connections),
      kMaxIpConnections(max_ip_connections) {}

TcpMonitor::~TcpMonitor() {}

bool TcpMonitor::OnConnection(const TcpConnectionPtr& p) {
  bool result = true;
  const std::string& ip = p->PeerSockAddr().Ip();
  mutex_.lock();
  if (++counter_ > kMaxAllConnections) {
    result = false;
    VOYAGER_LOG(WARN) << "the all connection size is " << counter_
                      << ", more than " << kMaxAllConnections
                      << ", so force to close it.";
  } else {
    auto it = ip_counter_.find(ip);
    if (it != ip_counter_.end()) {
      if (++(it->second) > kMaxIpConnections) {
        VOYAGER_LOG(WARN) << "the connection size of ip=" << ip << " is "
                          << it->second << ", more than " << kMaxIpConnections
                          << ", so force to close it.";
        result = false;
      }
    } else {
      ip_counter_.insert(std::make_pair(ip, 1));
    }
  }
  mutex_.unlock();

  if (!result) {
    p->ForceClose();
  }
  return result;
}

void TcpMonitor::OnClose(const TcpConnectionPtr& p) {
  const std::string& ip = p->PeerSockAddr().Ip();
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = ip_counter_.find(ip);
  if (it != ip_counter_.end()) {
    if (--(it->second) <= 0) {
      ip_counter_.erase(it);
    }
  }
  --counter_;
}

}  // namespace voyager
