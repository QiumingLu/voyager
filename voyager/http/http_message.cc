#include "voyager/http/http_message.h"

#include <string.h>

namespace voyager {

bool HttpMessage::SetVersion(const char* begin, const char* end) {
  size_t size = end - begin;
  if (strncasecmp(begin, "HTTP/1.0", size) == 0) {
    version_ = kHttp10;
  } else if (strncasecmp(begin, "HTTP/1.1", size) == 0) {
    version_ = kHttp11;
  } else if (strncasecmp(begin, "HTTP/2", size) == 0) {
    version_ = kHttp20;
  } else {
    return false;
  }
  return true;
}

const char* HttpMessage::VersionToString() const {
  const char* c;
  switch (version_) {
    case kHttp10:
      c = "HTTP/1.0";
      break;
    case kHttp11:
      c = "HTTP/1.1";
      break;
    case kHttp20:
      c = "HTTP/2";
      break;
    default:
      c = "";
      break;
  }
  return c;
}

void HttpMessage::AddHeader(const char* begin,
                            const char* colon,
                            const char* end) {
  std::string field(begin, colon);
  ++colon;
  while (colon != end && *colon == ' ') {
    ++colon;
  }
  std::string value(colon, end);
  header_map_[field] = value;
}

void HttpMessage::AddHeader(const std::string& field,
                            const std::string& value) {
  header_map_[field] = value;
}

void HttpMessage::RemoveHeader(const std::string& field) {
  header_map_.erase(field);
}

}  // namespace voyager
