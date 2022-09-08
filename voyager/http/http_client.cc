// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/http/http_client.h"

#include <string>

#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/http/http_response_parser.h"
#include "voyager/util/logging.h"

namespace voyager {

HttpClient::HttpClient(EventLoop* ev, uint64_t timeout)
    : eventloop_(ev), timeout_(timeout) {}

void HttpClient::DoHttpRequest(const HttpRequestPtr& request,
                               const RequestCallback& cb) {
  eventloop_->RunInLoop(
      [this, request, cb]() { this->DoHttpRequestInLoop(request, cb); });
}

void HttpClient::DoHttpRequestInLoop(const HttpRequestPtr& request,
                                     const RequestCallback& cb) {
  queue_cb_.push_back(cb);
  TcpConnectionPtr ptr(gaurd_.lock());
  if (ptr) {
    ptr->SendMessage(&request->RequestMessage());
  } else {
    FirstRequest(request);
  }
}

void HttpClient::FirstRequest(const HttpRequestPtr& request) {
  std::string host(request->Value(HttpMessage::kHost));
  uint16_t port = 80;
  size_t found = host.find(":");
  if (found != std::string::npos) {
    std::string p(host.begin() + found + 1, host.end());
    port = static_cast<uint16_t>(atoi(&*p.begin()));
    host.erase(host.begin() + found, host.end());
  }

  SockAddr addr(host, port);

  client_.reset(new TcpClient(eventloop_, addr));

  client_->SetConnectionCallback([this, request](const TcpConnectionPtr& ptr) {
    eventloop_->RemoveTimer(timer_);
    gaurd_ = ptr;
    ptr->SetContext(new HttpResponseParser());
    ptr->SendMessage(&request->RequestMessage());
  });

  client_->SetCloseCallback(
      std::bind(&HttpClient::HandleClose, this, std::placeholders::_1));
  client_->SetMessageCallback(std::bind(&HttpClient::HandleMessage, this,
                                        std::placeholders::_1,
                                        std::placeholders::_2));
  timer_ = eventloop_->RunAfter(timeout_,
                                std::bind(&HttpClient::HandleTimeout, this));
  client_->Connect();
}

void HttpClient::HandleMessage(const TcpConnectionPtr& ptr, Buffer* buffer) {
  assert(!queue_cb_.empty());
  HttpResponseParser* parser =
      reinterpret_cast<HttpResponseParser*>(ptr->Context());
  bool ok = parser->ParseBuffer(buffer);
  if (!ok) {
    ptr->ShutDown();
  }

  if (parser->FinishParse()) {
    RequestCallback cb = queue_cb_.front();
    queue_cb_.pop_front();
    cb(parser->GetResponse(), Status::OK());
    parser->Reset();
  }
}

void HttpClient::HandleClose(const TcpConnectionPtr& ptr) {
  HttpResponseParser* parser =
      reinterpret_cast<HttpResponseParser*>(ptr->Context());
  for (CallbackQueue::iterator it = queue_cb_.begin(); it != queue_cb_.end();
       ++it) {
    (*it)(nullptr, Status::NetworkError("Unknow error"));
  }
  queue_cb_.clear();
  delete parser;
}

void HttpClient::HandleTimeout() {
  if (!(gaurd_.lock()) && !(queue_cb_.empty())) {
    client_->Close();
    RequestCallback cb = queue_cb_.front();
    queue_cb_.pop_front();
    cb(nullptr, Status::NetworkError("Connect timeout"));
  }
}

}  // namespace voyager
