#include "voyager/http/http_server.h"

#include <functional>
#include <fstream>
#include <string>
#include <utility>
#include <iostream>
#include <sstream>  

#include "voyager/http/http_request.h"
#include "voyager/http/http_response.h"
#include "voyager/core/eventloop.h"

namespace voyager {

void HandleHttpRequest(const HttpRequest* request, HttpResponse* response) {
  response->SetStatusCode(HttpResponse::k200);
  response->SetReasonParse("OK");
  response->AddHeader("Content-Type", "text/html; charset=UTF-8");
  response->AddHeader("Content-Encoding", "UTF-8");
  response->AddHeader("Connection", "close");
  std::string s("Welcome to Voyager's WebServer!");
  response->SetBody(std::move(s));
}

}

int main() {
  voyager::EventLoop ev;
  voyager::SockAddr addr(5666);
  voyager::HttpServer server(&ev, addr, "WebServer", 4);
  server.SetHttpCallback(std::bind(voyager::HandleHttpRequest,
                                   std::placeholders::_1,
                                   std::placeholders::_2));
  server.Start();
  ev.Loop();
  return 0;
}
