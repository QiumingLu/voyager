#ifndef VOYAGER_EXAMPLES_HTTP_HTTPSERVER_H
#define VOYAGER_EXAMPLES_HTTP_HTTPSERVER_H

#include "voyager/core/tcp_server.h"

namespace http {

class Request;
class Response;

class HttpServer {
 public:
  typedef std::function<void (Request*, Response*)> HttpCallback;

  HttpServer(voyager::EventLoop* ev, 
             const voyager::SockAddr& addr,
             const std::string& name = std::string("MirantsServer"),
             int thread_size = 4);

  void Start();

  void SetHttpCallback(const HttpCallback& cb) {
    http_cb_ = cb;
  }

  void SetHttpCallback(HttpCallback&& cb) {
    http_cb_ = std::move(cb);
  }

 private:
  void ConnectCallback(const voyager::TcpConnectionPtr& ptr);
  void MessageCallback(const voyager::TcpConnectionPtr& ptr,
                       voyager::Buffer* buf);
  void DisConnectCallback(const voyager::TcpConnectionPtr& ptr);
  bool ProcessRequest(voyager::Buffer* buf, Request* request);
  bool ProcessRequestLine(const char* begin, 
                          const char* end,
                          Request* request);

  voyager::TcpServer server_;
  HttpCallback http_cb_;

  // No copying allow
  HttpServer(const HttpServer&);
  void operator= (const HttpServer&);
};

} // namespace http

#endif  // VOYAGER_EXAMPLES_HTTP_HTTPSERVER_H
