#include "voyager/http/http_request.h"

#include <string.h>

namespace voyager {

bool HttpRequest::SetMethod(const char* begin, const char* end) {
  size_t size = end - begin;
  if (strncasecmp(begin, "OPTIONS", size)) {
    method_ = kOptions;
  } else if (strncasecmp(begin, "HEAD", size)) {
    method_ = kHead;
  } else if (strncasecmp(begin, "GET", size)) {
    method_ = kGet;
  } else if (strncasecmp(begin, "POST", size)) {
    method_ = kPost;
  } else if (strncasecmp(begin, "PUT", size)) {
    method_ = kPut;
  } else if (strncasecmp(begin, "DELETE", size)) {
    method_ = kDelete;
  } else if (strncasecmp(begin, "TRACE", size)) {
    method_ = kTrace;
  } else if (strncasecmp(begin, "CONNECT", size)) {
    method_ = kConnect;
  } else if (strncasecmp(begin, "PATCH", size)) {
    method_ = kPatch;
  } else {
    method_ = kUnknown;
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

}  // namespace voyager
