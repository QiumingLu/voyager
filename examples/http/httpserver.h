#ifndef MIRANTS_EXAMPLES_HTTP_HTTPSERVER_H
#define MIRANTS_EXAMPLES_HTTP_HTTPSERVER_H

#include "mirants/core/tcp_server.h"

namespace http {

class Request;
class Response;

class HttpServer {
 public:
  typedef std::function<void (Request*, Response*)> HttpCallback;

  HttpServer(mirants::EventLoop* ev, 
             const mirants::SockAddr& addr,
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
  void ConnectCallback(const mirants::TcpConnectionPtr& ptr);
  void MessageCallback(const mirants::TcpConnectionPtr& ptr,
                       mirants::Buffer* buf);
  bool ProcessRequest(mirants::Buffer* buf, Request* request);
  bool ProcessRequestLine(const char* begin, 
                          const char* end,
                          Request* request);

  mirants::TcpServer server_;
  HttpCallback http_cb_;

  // No copying allow
  HttpServer(const HttpServer&);
  void operator= (const HttpServer&);
};

} // namespace http

#endif  // MIRANTS_EXAMPLES_HTTP_HTTPSERVER_H
