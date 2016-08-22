#include "examples/http/request.h"

namespace http {

std::string Request::MethodToString() const {
  const char* mt;
  switch (method_) {
    case kGet:
      mt = "GET";
      break;
    case kPost:
      mt = "POST";
      break;
    case kHead:
      mt = "HEAD";
      break;
    case kPut:
      mt = "PUT";
      break;
    case kDelete:
      mt = "DELETE";
      break;
    case kNotSupported:
      mt = "NOTSUPPORTED";
      break;
    default:
      mt = "NOTSUPPORTED";
  }
  std::string result(mt);
  return result;
}

std::string Request::VersionToString() const {
  const char* v;
  switch (version_) {
    case kHttp10:
      v = "HTTP/1.0";
      break;
    case kHttp11:
      v = "HTTP/1.1";
      break;
    case kUnknown:
      v = "";
      break;
    default:
      v = "";
  }
  std::string result(v);
  return result;
}

}  // namespace http
