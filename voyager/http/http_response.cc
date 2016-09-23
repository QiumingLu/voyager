#include "voyager/http/http_response.h"
#include <map>

namespace voyager {

Buffer& HttpResponse::ResponseMessage() {
  message_.Append(VersionToString());
  message_.Append(" ");
  message_.Append(status_code_);
  message_.Append(" ");
  message_.Append(reason_parse_);
  message_.Append("\r\n");
  for (std::map<std::string, std::string>::iterator it = header_map_.begin();
       it != header_map_.end(); ++it) {
    message_.Append(it->first);
    message_.Append(": ");
    message_.Append(it->second);
    message_.Append("\r\n");
  }
  message_.Append("\r\n");
  message_.Append(body_);
  return message_;
}

}  // namespace voyager
