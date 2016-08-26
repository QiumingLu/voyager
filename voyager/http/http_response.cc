#include "voyager/http/http_response.h"

namespace voyager {

std::string HttpResponse::StatusCodeToString() const {
  const char* c;
  switch (status_code_) {
    case k200:
      c = "HTTP/1.1 200 OK\r\n";
      break;
    case k301:
      c = "HTTP/1.1 301 Moved Permanently\r\n";
      break;
    case k400:
      c = "HTTP/1.1 404 BAD REQUEST\r\n";
      break;
    case k404:
      c = "HTTP/1.1 404 NOT FOUND\r\n";
      break;
    case k500:
      c = "HTTP/1.1 500 Interval Server Error\r\n";
      break;
    case k501:
      c = "HTTP/1.1 501 Method Not Implemented\r\n";
      break;
    default:
      c = "HTTP/1.1 Unknown Error\r\n";
  }
  std::string result(c);
  return result;
}

}  // namespace voyager
