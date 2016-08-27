#ifndef VOYAGER_HTTP_HTTP_RESPONSE_H_
#define VOYAGER_HTTP_HTTP_RESPONSE_H_

#include <string>

#include "voyager/http/http_message.h"
#include "voyager/core/buffer.h"

namespace voyager {

class HttpResponse : public HttpMessage {
 public:
  enum StatusCode {
    k200,
    k301,
    k400,
    k404,
    k500,
    k501,
    kUnknown,
  };

  HttpResponse()
      :  status_code_(k200), reason_parse_("OK") {
  }

  void SetCloseState(bool close) { close_ =  close; }
  bool CloseState() const { return close_; }

  void SetStatusCode(StatusCode code) { status_code_ = code; }
  StatusCode GetStatusCode() const { return status_code_; }
  const char* StatusCodeToString() const;

  void SetReasonParse(const std::string& s) { reason_parse_ = s; }
  const std::string& ReasonParse() const { return reason_parse_; }

  Buffer& ResponseMessage();

 private:
  bool close_;
  StatusCode status_code_;
  std::string reason_parse_;
  Buffer message_;
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_RESPONSE_H_
