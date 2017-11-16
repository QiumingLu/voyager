// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/tcp_monitor.h"

#include <assert.h>

#include "voyager/util/logging.h"

namespace voyager {

TcpMonitor::TcpMonitor(int max_all_connections, int max_ip_connections)
    : kMaxAllConnections(max_all_connections),
      kMaxIpConnections(max_ip_connections),
      counter_(0) {}

TcpMonitor::~TcpMonitor() {}

void TcpMonitor::AddAllowIp(const std::string& ip) {
  std::lock_guard<std::mutex> lock(mutex_);
  allow_ip_set_.insert(ip);
}

void TcpMonitor::DeleteAllowIp(const std::string& ip) {
  std::lock_guard<std::mutex> lock(mutex_);
  allow_ip_set_.erase(ip);
}

void TcpMonitor::AddDenyIp(const std::string& ip) {
  std::lock_guard<std::mutex> lock(mutex_);
  deny_ip_set_.insert(ip);
}

void TcpMonitor::DeleteDenyIp(const std::string& ip) {
  std::lock_guard<std::mutex> lock(mutex_);
  deny_ip_set_.erase(ip);
}

bool TcpMonitor::OnConnection(const TcpConnectionPtr& p) {
  bool result = true;
  const std::string& ip = p->PeerSockAddr().Ip();
  mutex_.lock();
  if (++counter_ > kMaxAllConnections) {
    result = false;
    VOYAGER_LOG(WARN) << "the all connection size is " << counter_
                      << ", more than " << kMaxAllConnections
                      << ", so force to close it.";
  }
  if (kMaxIpConnections != 0) {
    auto it = ip_counter_.find(ip);
    if (it != ip_counter_.end()) {
      ++(it->second);
      if (it->second > kMaxIpConnections &&
          allow_ip_set_.find(ip) == allow_ip_set_.end()) {
        VOYAGER_LOG(WARN) << "the connection size of ip=" << ip << " is "
                          << it->second << ", more than " << kMaxIpConnections
                          << ", so force to close it.";
        result = false;
      }
    } else {
      ip_counter_.insert(std::make_pair(ip, 1));
    }
  }
  if (result && deny_ip_set_.find(ip) != deny_ip_set_.end()) {
    result = false;
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
  if (kMaxIpConnections != 0) {
    assert(ip_counter_.find(ip) != ip_counter_.end());
    auto it = ip_counter_.find(ip);
    if (--(it->second) <= 0) {
      ip_counter_.erase(it);
    }
  }
  --counter_;
}

}  // namespace voyager
