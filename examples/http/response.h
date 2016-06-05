#ifndef MIRANTS_EXAMPLES_HTTP_RESPONSE_H_
#define MIRANTS_EXAMPLES_HTTP_RESPONSE_H_

#include <string>
#include <map>
#include "mirants/core/buffer.h"

namespace http {


class Response {
 public:
  enum HttpStatusCode {
    k200 = 200,
    k301 = 301,
    k400 = 400,
    k404 = 404,
    k500 = 500,
    k501 = 501,
    kUnknown
  };

  explicit Response(bool close)
    : close_(close)
  { }

  void set_close_connection(bool close) {
    close_ = close;
  }

  bool close_connection() const {
    return close_;
  }

  void set_status_code(HttpStatusCode code) {
    code_ = code;
  }

  void set_body(const std::string& body) {
    body_ = body;
  }

  void set_headers(const std::map<std::string, std::string>& headers) {
    headers_ = headers;
  }

  void set_headers(std::map<std::string, std::string>&& headers) {
    headers_ = headers;
  }
  
  void AppendToBuffer(mirants::Buffer* buf);

 private:
  std::string StatusCodeToString() const;

  HttpStatusCode code_;
  std::map<std::string, std::string> headers_;
  std::string body_;
  bool close_;
};

}  // namespace http

#endif  // MIRANTS_EXAMPLES_HTTP_RESPONSE_H_
