#ifndef VOYAGER_HTTP_HTTP_RESPONSE_H_
#define VOYAGER_HTTP_HTTP_RESPONSE_H_

#include <string>

#include "voyager/http/http_message.h"

namespace voyager {

class HttpResponse : public HttpMessage {
 public:
  enum StatusCode {
    k200 = 0,
    k301 = 1,
    k400 = 2,
    k404 = 3,
    k500 = 4,
    k501 = 5,
    kUnknown = 6
  };

  void SetStatusCode(StatusCode code) { status_code_ = code; }
  StatusCode GetStatusCode() const { return status_code_; }

  std::string StatusCodeToString() const;

 private:
  StatusCode status_code_;
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_RESPONSE_H_
