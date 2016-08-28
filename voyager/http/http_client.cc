#include "voyager/http/http_client.h"

#include <stdio.h>

#include "voyager/http/http_parser.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/util/logging.h"

namespace voyager {

HttpClient::HttpClient(EventLoop* ev)
    : eventloop_(ev) {
}

void HttpClient::DoHttpRequest(const HttpRequestPtr& request) {
  eventloop_->RunInLoop([this, request]() {
    this->DoHttpRequestInLoop(request);
  });
}

void HttpClient::DoHttpRequestInLoop(const HttpRequestPtr& request) {
  TcpConnectionPtr ptr(gaurd_.lock());
  if (ptr) {
    ptr->SendMessage(&request->RequestMessage());
  } else {
    FirstRequest(request);
  }
}

void HttpClient::FirstRequest(const HttpRequestPtr& request) {
  std::string host = request->Value(HttpMessage::kHost);
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
    this->gaurd_ = ptr;
    ptr->SetUserData(new HttpParser(HttpParser::kHttpResponse));
    ptr->SendMessage(&request->RequestMessage());
  });

  client_->SetCloseCallback([](const TcpConnectionPtr& ptr) {
    HttpParser* parser = reinterpret_cast<HttpParser*>(ptr->UserData());
    delete parser;
  });

  client_->SetMessageCallback([this](const TcpConnectionPtr& ptr,
                                    Buffer* buffer) {
    if (request_cb_) {
      HttpParser* parser = reinterpret_cast<HttpParser*>(ptr->UserData());
      parser->ParseBuffer(buffer);
      if (parser->FinishParse()) {
        request_cb_(parser->GetResponse());
      }
    }
  });

  client_->Connect();
}

}  // namespace voyager
