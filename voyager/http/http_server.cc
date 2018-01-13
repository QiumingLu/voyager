// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/http/http_server.h"
#include "voyager/http/http_request_parser.h"

namespace voyager {

struct HttpServer::Context {
  explicit Context(const EntryPtr& e) : entry_wp(e) {}
  std::weak_ptr<Entry> entry_wp;
  HttpRequestParser parser;
};

struct HttpServer::Entry {
  explicit Entry(const TcpConnectionPtr& p) : index(-1), conn_wp(p) {}
  ~Entry() {
    TcpConnectionPtr p = conn_wp.lock();
    if (p) {
      p->ShutDown();
    }
  }
  int index;
  std::weak_ptr<TcpConnection> conn_wp;
};

HttpServer::HttpServer(EventLoop* ev, const HttpServerOptions& options)
    : options_(options),
      idle_ticks_(options.keep_alive_time_out / options.tick_time),
      monitor_(options.max_all_connections, options.max_ip_connections),
      server_(ev, SockAddr(options.host, options.port), "HttpServer",
              options.thread_size) {
  server_.SetConnectionCallback(
      std::bind(&HttpServer::OnConnection, this, std::placeholders::_1));
  server_.SetCloseCallback(
      std::bind(&HttpServer::OnClose, this, std::placeholders::_1));
  server_.SetMessageCallback(std::bind(&HttpServer::OnMessage, this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
}

void HttpServer::Start() {
  server_.Start();
  if (idle_ticks_ > 0) {
    const std::vector<EventLoop*>* loops = server_.AllLoops();
    for (auto& loop : *loops) {
      buckets_.insert(
          std::make_pair(loop, std::make_pair(BucketList(idle_ticks_), 0)));
      loop->RunEvery(options_.tick_time, std::bind(&HttpServer::OnTimer, this));
    }
  }
}

void HttpServer::OnConnection(const TcpConnectionPtr& ptr) {
  bool result = monitor_.OnConnection(ptr);
  if (result) {
    EntryPtr entry = std::make_shared<Entry>(ptr);
    UpdateBuckets(ptr, entry);
    ptr->SetContext(new Context(entry));
  }
}

void HttpServer::OnClose(const TcpConnectionPtr& ptr) {
  monitor_.OnClose(ptr);
  Context* context = reinterpret_cast<Context*>(ptr->Context());
  delete context;
}

void HttpServer::OnMessage(const TcpConnectionPtr& ptr, Buffer* buf) {
  Context* context = reinterpret_cast<Context*>(ptr->Context());
  HttpRequestParser& parser = context->parser;
  bool ok = parser.ParseBuffer(buf);
  if (!ok) {
    ptr->SendMessage(std::string("HTTP/1.1 400 Bad Request\r\n\r\n"));
    ptr->ShutDown();
  }

  if (parser.FinishParse()) {
    HttpRequestPtr request(parser.GetRequest());
    HttpResponse response;

    std::string s(request->Value(HttpMessage::kConnection));

    if (strcasecmp(&*s.begin(), "close") == 0 ||
        (strcasecmp(&*s.begin(), "keep-alive") != 0 &&
         (request->Version() == HttpMessage::kHttp10))) {
      response.SetCloseState(true);
    } else {
      parser.Reset();
    }

    if (http_cb_) {
      http_cb_(request, &response);
    }

    ptr->SendMessage(&response.ResponseMessage());
    if (response.CloseState()) {
      ptr->ShutDown();
    }
  }

  if (ptr->IsConnected()) {
    EntryPtr entry = (context->entry_wp).lock();
    if (entry) {
      UpdateBuckets(ptr, entry);
    }
  }
}

void HttpServer::OnTimer() {
  auto it = buckets_.find(EventLoop::RunLoop());
  assert(it != buckets_.end());
  if (++(it->second.second) == idle_ticks_) {
    it->second.second = 0;
  }
  it->second.first.at(it->second.second).clear();
}

void HttpServer::UpdateBuckets(const TcpConnectionPtr& ptr,
                               const EntryPtr& entry) {
  auto it = buckets_.find(ptr->OwnerEventLoop());
  assert(it != buckets_.end());
  if (entry->index != it->second.second) {
    it->second.first.at(it->second.second).insert(entry);
    entry->index = it->second.second;
  }
}

}  // namespace voyager
