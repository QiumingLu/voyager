#ifndef VOYAGER_HTTP_HTTP_CLIENT_H_
#define VOYAGER_HTTP_HTTP_CLIENT_H_

#include <functional>
#include <memory>
#include <deque>

#include "voyager/http/http_request.h"
#include "voyager/http/http_response.h"
#include "voyager/core/tcp_client.h"

namespace voyager {

class HttpClient {
 public:
  typedef std::function<void (HttpResponsePtr,
                              const Status&)> RequestCallback;
  explicit HttpClient(EventLoop* ev, int timeout = 30);

  void DoHttpRequest(const HttpRequestPtr& request,
                     const RequestCallback& cb);

 private:
  void DoHttpRequestInLoop(const HttpRequestPtr& request,
                           const RequestCallback& cb);
  void FirstRequest(const HttpRequestPtr& request);

  EventLoop* eventloop_;
  int timeout_;
  std::weak_ptr<TcpConnection> gaurd_;
  std::unique_ptr<TcpClient> client_;
  typedef std::deque<RequestCallback> CallbackQueue;
  CallbackQueue queue_cb_;

  // No copying allowed
  HttpClient(const HttpClient&);
  void operator=(const HttpClient&);
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_CLIENT_H_
