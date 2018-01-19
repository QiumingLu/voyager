// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_HTTP_HTTP_SERVER_H_
#define VOYAGER_HTTP_HTTP_SERVER_H_

#include <functional>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "voyager/core/eventloop.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/core/tcp_monitor.h"
#include "voyager/core/tcp_server.h"
#include "voyager/http/http_request.h"
#include "voyager/http/http_response.h"
#include "voyager/http/http_server_options.h"

namespace voyager {

class HttpServer {
 public:
  typedef std::function<void(HttpRequestPtr, HttpResponse*)> HttpCallback;

  HttpServer(EventLoop* ev, const HttpServerOptions& options);

  void Start();

  void SetHttpCallback(const HttpCallback& cb) { http_cb_ = cb; }
  void SetHttpCallback(HttpCallback&& cb) { http_cb_ = std::move(cb); }

 private:
  struct Context;
  struct Entry;
  typedef std::shared_ptr<Entry> EntryPtr;
  typedef std::vector<EntryPtr> Bucket;
  typedef std::vector<Bucket> BucketList;

  void OnConnection(const TcpConnectionPtr& ptr);
  void OnClose(const TcpConnectionPtr& ptr);
  void OnMessage(const TcpConnectionPtr& ptr, Buffer* buf);
  void OnTimer();
  void UpdateBuckets(const TcpConnectionPtr& ptr, const EntryPtr& entry);

  HttpServerOptions options_;
  HttpCallback http_cb_;

  int idle_ticks_;
  std::map<voyager::EventLoop*, std::pair<BucketList, int>> buckets_;

  TcpMonitor monitor_;
  TcpServer server_;

  HttpServer(const HttpServer&);
  void operator=(const HttpServer&);
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_SERVER_H_
