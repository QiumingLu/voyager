#include "voyager/http/http_response.h"

namespace voyager {

const char* HttpResponse::StatusCodeToString() const {
  const char* c;
  switch (status_code_) {
    case k200:
      c = "200";
      break;
    case k301:
      c = "301";
      break;
    case k400:
      c = "404";
      break;
    case k404:
      c = "404";
      break;
    case k500:
      c = "500";
      break;
    case k501:
      c = "501";
      break;
    default:
      c = "404";
  }
  return c;
}

Buffer& HttpResponse::ResponseMessage() {
  message_.Append(VersionToString());
  message_.Append(" ");
  message_.Append(StatusCodeToString());
  message_.Append(" ");
  message_.Append(reason_parse_);
  message_.Append("\r\n");
  for (std::map<std::string, std::string>::iterator it = header_map_.begin();
       it != header_map_.end(); ++it) {
    message_.Append(it->first);
    message_.Append(" ");
    message_.Append(it->second);
    message_.Append("\r\n");
  }
  message_.Append("\r\n");
  message_.Append(body_);
  message_.Append("\r\n");
  return message_;
}

}  // namespace voyager
