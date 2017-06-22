// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_HTTP_HTTP_CLIENT_H_
#define VOYAGER_HTTP_HTTP_CLIENT_H_

#include <deque>
#include <functional>
#include <memory>

#include "voyager/core/tcp_client.h"
#include "voyager/core/timerlist.h"
#include "voyager/http/http_request.h"
#include "voyager/http/http_response.h"
#include "voyager/util/status.h"

namespace voyager {

// Noly write for test, which cann't use in produce environment!
class HttpClient {
 public:
  typedef std::function<void(HttpResponsePtr, const Status&)> RequestCallback;
  explicit HttpClient(EventLoop* ev, uint64_t timeout = 15);

  void DoHttpRequest(const HttpRequestPtr& request, const RequestCallback& cb);

 private:
  void DoHttpRequestInLoop(const HttpRequestPtr& request,
                           const RequestCallback& cb);
  void FirstRequest(const HttpRequestPtr& request);

  void HandleMessage(const TcpConnectionPtr& ptr, Buffer* buffer);
  void HandleClose(const TcpConnectionPtr& ptr);
  void HandleTimeout();

  EventLoop* eventloop_;
  uint64_t timeout_;
  TimerId timer_;
  std::weak_ptr<TcpConnection> gaurd_;
  std::unique_ptr<TcpClient> client_;
  typedef std::deque<RequestCallback> CallbackQueue;
  CallbackQueue queue_cb_;

  // No copying allowed
  HttpClient(const HttpClient&);
  void operator=(const HttpClient&);
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_CLIENT_H_
