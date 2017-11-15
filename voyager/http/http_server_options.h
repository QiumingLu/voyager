// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_HTTP_HTTP_SERVER_OPTIONS_H_
#define VOYAGER_HTTP_HTTP_SERVER_OPTIONS_H_

#include <stdint.h>
#include <string>

namespace voyager {

struct HttpServerOptions {
  // Default: "127.0.0.1"
  std::string host;

  // Default: 8080
  uint16_t port;

  // Default: 0
  int thread_size;

  // Default: 2 * 1000 * 1000 microseconds, which must greater than zero.
  int tick_time;

  // Default: 5 * tick_time
  // If the value less than or equal to zero, it means that the keep alive
  // timeout is no set.
  int keep_alive_time_out;

  // Default: 60000
  int max_all_connections;

  // Default: 60
  int max_ip_connections;

  HttpServerOptions();
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_SERVER_OPTIONS_H_
