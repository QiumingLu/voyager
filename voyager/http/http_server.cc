#include "voyager/http/http_server.h"
#include "voyager/http/http_parser.h"
#include "voyager/http/http_request.h"
#include "voyager/http/http_response.h"
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
  ptr->SetUserData(new HttpParser());
}

void HttpServer::HandleClose(const TcpConnectionPtr& ptr) {
  HttpParser* parser = reinterpret_cast<HttpParser*>(ptr->UserData());
  delete parser;
}

void HttpServer::HandleMessage(const TcpConnectionPtr& ptr, Buffer* buf) {
  HttpParser* parser = reinterpret_cast<HttpParser*>(ptr->UserData());
  bool ok = parser->ParseBuffer(buf);
  if (!ok) {
    ptr->SendMessage(std::string("HTTP/1.1 400 Bad Request\r\n\r\n"));
    ptr->ShutDown();
  }

  if (parser->FinishParse()) {
    HttpRequest* request = parser->GetRequest();
    HttpResponse response;

    const std::string& s = request->Value("Connection");
    if (s == "close" ||
        s != "Keep-Alive" ||
        (request->Version() == HttpRequest::kHttp10)) {
      response.SetCloseState(true);
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
