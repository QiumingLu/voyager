// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/http/http_server_options.h"

namespace voyager {

HttpServerOptions::HttpServerOptions()
    : host("127.0.0.1"),
      port(5666),
      thread_size(0),
      tick_time(2000000),
      keep_alive_time_out(5 * tick_time),
      max_all_connections(60000),
      max_ip_connections(60) {}

}  // namespace voyager
