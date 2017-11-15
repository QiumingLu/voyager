// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_TCP_MONITOR_H_
#define VOYAGER_CORE_TCP_MONITOR_H_

#include <mutex>
#include <string>
#include <unordered_map>

#include "voyager/core/tcp_connection.h"

namespace voyager {

class TcpMonitor {
 public:
  TcpMonitor(int max_all_connections, int max_ip_connections);
  ~TcpMonitor();

  bool OnConnection(const TcpConnectionPtr& p);
  void OnClose(const TcpConnectionPtr& p);

 private:
  const int kMaxAllConnections;
  const int kMaxIpConnections;

  std::mutex mutex_;
  int counter_;
  std::unordered_map<std::string, int> ip_counter_;

  // No copying allowed
  TcpMonitor(const TcpMonitor&);
  void operator=(const TcpMonitor&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_MONITOR_H_
