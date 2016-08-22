#include "examples/http/response.h"

namespace http {

void Response::AppendToBuffer(voyager::Buffer* buf) {
  buf->Append(StatusCodeToString());
  if (close_) {
    buf->Append("Connection: close\r\n");
  } else {
    char c[32];
    snprintf(c, sizeof(c), "Content-Length: %zd\r\n", body_.size());
    buf->Append(c);
    buf->Append("Connection: Keep-Alive\r\n");
  }

  for (std::map<std::string, std::string>::const_iterator it = headers_.begin();
       it != headers_.end();
       ++it) {
    buf->Append(it->first);
    buf->Append(": ");
    buf->Append(it->second);
    buf->Append("\r\n");
  }

  buf->Append("\r\n");
  buf->Append(body_);
}

std::string Response::StatusCodeToString() const {
  const char* c;
  switch (code_) {
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

  std::string s(c);
  return s;
}

}  // namespace http
