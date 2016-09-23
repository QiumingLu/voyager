#include "voyager/http/http_client.h"

#include <string>

#include "voyager/http/http_response_parser.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/util/logging.h"

namespace voyager {

HttpClient::HttpClient(EventLoop* ev, int timeout)
    : eventloop_(ev),
      timeout_(timeout) {
}

void HttpClient::DoHttpRequest(const HttpRequestPtr& request,
                               const RequestCallback& cb) {
  eventloop_->RunInLoop([this, request, cb]() {
    this->DoHttpRequestInLoop(request, cb);
  });
}

void HttpClient::DoHttpRequestInLoop(const HttpRequestPtr& request,
                                     const RequestCallback& cb) {
  queue_cb_.push_back(cb);
  TcpConnectionPtr ptr(gaurd_.lock());
  if (ptr) {
    ptr->SendMessage(&request->RequestMessage());
  } else {
    FirstRequest(request);
  }
}

void HttpClient::FirstRequest(const HttpRequestPtr& request) {
  std::string host(request->Value(HttpMessage::kHost));
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
    ptr->SetUserData(new HttpResponseParser());
    ptr->SendMessage(&request->RequestMessage());
  });

  client_->SetCloseCallback([this](const TcpConnectionPtr& ptr) {
    HttpResponseParser* parser
        = reinterpret_cast<HttpResponseParser*>(ptr->UserData());
    for (CallbackQueue::iterator it = queue_cb_.begin();
         it != queue_cb_.end(); ++it) {
      (*it)(nullptr, Status::NetworkError("Unknow error"));
    }
    queue_cb_.clear();
    delete parser;
  });

  client_->SetMessageCallback([this](const TcpConnectionPtr& ptr,
                                    Buffer* buffer) {
    assert(!queue_cb_.empty());
    HttpResponseParser* parser
        = reinterpret_cast<HttpResponseParser*>(ptr->UserData());
    parser->ParseBuffer(buffer);
    if (parser->FinishParse()) {
      RequestCallback cb = queue_cb_.front();
      queue_cb_.pop_front();
      cb(parser->GetResponse(), Status::OK());
      parser->Reset();
    }
  });

#ifdef __linux__
    if (!timer_) {
      timer_.reset(new NewTimer(eventloop_,
                                std::bind(&HttpClient::HandleTimeout, this)));
    }
    timer_->SetTime(timeout_*1000000000, 0);
#else
    eventloop_->RunAfter(timeout_*1000000,
                         std::bind(&HttpClient::HandleTimeout, this));
#endif

  client_->Connect();
}

void HttpClient::HandleTimeout() {
  if (!gaurd_.lock()) {
    client_->Close();
    RequestCallback cb = queue_cb_.front();
    queue_cb_.pop_front();
    cb(nullptr, Status::NetworkError("Connect timeout"));
  }
}

}  // namespace voyager
