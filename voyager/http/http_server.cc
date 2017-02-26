// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/http/http_server.h"
#include "voyager/http/http_response.h"
#include "voyager/http/http_request_parser.h"
#include "voyager/core/tcp_connection.h"

namespace voyager {

HttpServer::HttpServer(EventLoop* ev,
                       const SockAddr& addr,
                       const std::string& name,
                       int thread_size)
    : server_(ev, addr, name, thread_size) {
  server_.SetConnectionCallback(std::bind(&HttpServer::NewConnection,
                                          this,
                                          std::placeholders::_1));
  server_.SetCloseCallback(std::bind(&HttpServer::HandleClose,
                                     this,
                                     std::placeholders::_1));
  server_.SetMessageCallback(std::bind(&HttpServer::HandleMessage,
                                       this,
                                       std::placeholders::_1,
                                       std::placeholders::_2));
}

void HttpServer::Start() {
  server_.Start();
}

void HttpServer::NewConnection(const TcpConnectionPtr& ptr) {
  ptr->SetUserData(new HttpRequestParser());
}

void HttpServer::HandleClose(const TcpConnectionPtr& ptr) {
  HttpRequestParser* parser
      = reinterpret_cast<HttpRequestParser*>(ptr->UserData());
  delete parser;
}

void HttpServer::HandleMessage(const TcpConnectionPtr& ptr, Buffer* buf) {
  HttpRequestParser* parser
      = reinterpret_cast<HttpRequestParser*>(ptr->UserData());
  bool ok = parser->ParseBuffer(buf);
  if (!ok) {
    ptr->SendMessage(std::string("HTTP/1.1 400 Bad Request\r\n\r\n"));
    ptr->ShutDown();
  }

  if (parser->FinishParse()) {
    HttpRequestPtr request(parser->GetRequest());
    HttpResponse response;

    std::string s(request->Value(HttpMessage::kConnection));

    if (strcasecmp(&*s.begin(), "close") == 0 ||
        (strcasecmp(&*s.begin(), "keep-alive") != 0 &&
        (request->Version() == HttpMessage::kHttp10))) {
      response.SetCloseState(true);
    } else {
      parser->Reset();
    }

    if (http_cb_) {
      http_cb_(request, &response);
    }

    ptr->SendMessage(&response.ResponseMessage());
    if (response.CloseState()) {
      ptr->ShutDown();
    }
  }
}

}  // namespace voyager
