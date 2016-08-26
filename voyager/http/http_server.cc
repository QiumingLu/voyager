#include "voyager/http/http_server.h"

#include "voyager/http/http_request.h"

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
}

void HttpServer::NewConnection(const TcpConnectionPtr& ptr) {
  ptr->SetUserData(std::shared_ptr<HttpRequest>(new HttpRequest));
}

void HttpServer::HandleClose(const TcpConnectionPtr& ptr) {
}

void HttpServer::HandleMessage(const TcpConnectionPtr& ptr, Buffer* buf) {
  std::shared_ptr<HttpRequest> request = ptr->UserData();
}

}  // namespace voyager
