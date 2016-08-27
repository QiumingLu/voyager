#ifndef VOYAGER_HTTP_HTTP_CLIENT_H_
#define VOYAGER_HTTP_HTTP_CLIENT_H_

#include <functional>
#include <memory>

#include "voyager/http/http_request.h"
#include "voyager/http/http_response.h"
#include "voyager/core/tcp_client.h"

namespace voyager {

class HttpClient {
 public:
  typedef std::function<void (HttpResponse*)> RequestCallback;

  HttpClient(EventLoop* ev);

  void SetRequestCallback(const RequestCallback& cb) { request_cb_ = cb; }
  void SetRequestCallback(RequestCallback&& cb) { request_cb_ = std::move(cb); }

  void DoHttpRequest(HttpRequest* request);

 private:
  void DoHttpRequestInLoop(HttpRequest* request);
  void Init(HttpRequest* request);

  EventLoop* eventloop_;
  std::weak_ptr<TcpConnection> gaurd_;
  std::unique_ptr<TcpClient> client_;
  RequestCallback request_cb_;

  // No copyiny allow
  HttpClient(const HttpClient&);
  void operator=(const HttpClient&);
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_CLIENT_H_
