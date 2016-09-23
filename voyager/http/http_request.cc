#include "voyager/http/http_request.h"

#include <string.h>
#include <map>

namespace voyager {

HttpRequest::HttpRequest() {
}

bool HttpRequest::SetMethod(const char* begin, const char* end) {
  size_t size = end - begin;
  if (strncasecmp(begin, "OPTIONS", size) == 0) {
    method_ = kOptions;
  } else if (strncasecmp(begin, "HEAD", size) == 0) {
    method_ = kHead;
  } else if (strncasecmp(begin, "GET", size) == 0) {
    method_ = kGet;
  } else if (strncasecmp(begin, "POST", size) == 0) {
    method_ = kPost;
  } else if (strncasecmp(begin, "PUT", size) == 0) {
    method_ = kPut;
  } else if (strncasecmp(begin, "DELETE", size) == 0) {
    method_ = kDelete;
  } else if (strncasecmp(begin, "TRACE", size) == 0) {
    method_ = kTrace;
  } else if (strncasecmp(begin, "CONNECT", size) == 0) {
    method_ = kConnect;
  } else if (strncasecmp(begin, "PATCH", size) == 0) {
    method_ = kPatch;
  } else {
    return false;
  }
  return true;
}

const char* HttpRequest::MethodToString() const {
  const char* c;
  switch (method_) {
    case kOptions:
      c = "OPTIONS";
      break;

    case kHead:
      c = "HEAD";
      break;

    case kGet:
      c = "GET";
      break;

    case kPost:
      c = "POST";
      break;

    case kPut:
      c = "PUT";
      break;

    case kDelete:
      c = "DELETE";
      break;

    case kTrace:
      c = "TRACE";
      break;

    case kConnect:
      c = "CONNECT";
      break;

    case kPatch:
      c = "PATCH";
      break;

    default:
      c = "UNKNOWN";
      break;
  }

  return c;
}

Buffer& HttpRequest::RequestMessage() {
  message_.Append(MethodToString());
  message_.Append(" ");
  message_.Append(path_);
  if (!query_.empty()) {
    message_.Append("?");
    message_.Append(query_);
  }
  message_.Append(" ");
  message_.Append(VersionToString());
  message_.Append("\r\n");
  for (std::map<std::string, std::string>::iterator it = header_map_.begin();
       it != header_map_.end(); ++it) {
    message_.Append(it->first);
    message_.Append(":");
    message_.Append(it->second);
    message_.Append("\r\n");
  }
  message_.Append("\r\n");
  message_.Append(body_);

  return message_;
}

}  // namespace voyager
