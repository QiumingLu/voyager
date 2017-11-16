// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_TCP_MONITOR_H_
#define VOYAGER_CORE_TCP_MONITOR_H_

#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "voyager/core/tcp_connection.h"

namespace voyager {

class TcpMonitor {
 public:
  explicit TcpMonitor(int max_all_connections, int max_ip_connections = 0);
  ~TcpMonitor();

  void SetAllowIpSet(const std::unordered_set<std::string>& allow_ip_set) {
    allow_ip_set_ = allow_ip_set;
  }
  void SetAllowIpSet(std::unordered_set<std::string>&& allow_ip_set) {
    allow_ip_set_ = std::move(allow_ip_set);
  }
  void SetDenyIpSet(const std::unordered_set<std::string>& deny_ip_set) {
    deny_ip_set_ = deny_ip_set;
  }
  void SetDenyIpSet(std::unordered_set<std::string>&& deny_ip_set) {
    deny_ip_set_ = std::move(deny_ip_set);
  }

  void AddAllowIp(const std::string& ip);
  void DeleteAllowIp(const std::string& ip);
  void AddDenyIp(const std::string& ip);
  void DeleteDenyIp(const std::string& ip);

  bool OnConnection(const TcpConnectionPtr& p);
  void OnClose(const TcpConnectionPtr& p);

 private:
  const int kMaxAllConnections;
  const int kMaxIpConnections;

  std::mutex mutex_;
  int counter_;
  std::unordered_set<std::string> allow_ip_set_;
  std::unordered_set<std::string> deny_ip_set_;
  std::unordered_map<std::string, int> ip_counter_;

  // No copying allowed
  TcpMonitor(const TcpMonitor&);
  void operator=(const TcpMonitor&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_MONITOR_H_
