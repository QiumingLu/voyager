#include "voyager/http/http_client.h"

#include <stdlib.h>

#include "voyager/http/http_parser.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"

namespace voyager {

HttpClient::HttpClient(EventLoop* ev)
    : eventloop_(ev) {
}

void HttpClient::DoHttpRequest(HttpRequest* request) {
  eventloop_->RunInLoop([this, request]() {
    this->DoHttpRequestInLoop(request);
  });
}

void HttpClient::DoHttpRequestInLoop(HttpRequest* request) {
  TcpConnectionPtr ptr(gaurd_.lock());
  if (ptr) {
    ptr->SendMessage(&request->RequestMessage());
  } else {
    Init(request);
  }
}

void HttpClient::Init(HttpRequest* request) {
  const std::string& path = request->Path();
  std::size_t found = path.find("/");
  std::string host;
  if (found != std::string::npos) {
    host = std::string(path, 0, found);
  } else {
    host = path;
  }
  std::string p;
  found = host.find(":");
  if (found != std::string::npos) {
    p = std::string(host.begin() + found + 1, host.end());
    host.erase(host.begin() + found, host.end());
  } else {
    p = "80";
  }

  uint16_t port = static_cast<uint16_t>(atoi(&*p.begin()));
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
      request_cb_(parser->GetResponse());
    }
  });

  client_->Connect();
}

}  // namespace voyager
