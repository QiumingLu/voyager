#include "voyager/http/http_message.h"

#include <utility>

namespace voyager {

std::string HttpMessage::VersionToString() const {
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
  }
  std::string result(c);
  return result;
}

void HttpMessage::AppendContent(const Slice& s) {
  content_.append(s.data(), s.size());
}

void HttpMessage::SetContent(const std::string& s) {
  content_ = s;
}

void HttpMessage::SetContent(std::string&& s) {
  content_ = std::move(s);
}

void HttpMessage::AddHeader(const char* begin,
                            const char* colon,
                            const char* end) {
  std::string field(begin, colon);
  ++colon;
  while (colon < end && isspace(*colon)) {
    ++colon;
  }
  const char* tmp = end;
  --tmp;
  while (isspace(*tmp)) {
    --tmp;
  }
  ++tmp;
  std::string value(colon, tmp);
  AddHeader(field, value);
}

void HttpMessage::AddHeader(const std::string& field,
                            const std::string& value) {
  header_map_[field] = value;
}

void HttpMessage::RemoveHeader(const std::string& field) {
  header_map_.erase(field);
}

}  // namespace voyager
