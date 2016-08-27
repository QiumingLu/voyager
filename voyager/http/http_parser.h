#ifndef VOYAGER_HTTP_HTTP_PARSER_H_
#define VOYAGER_HTTP_HTTP_PARSER_H_

#include "voyager/http/http_request.h"
#include "voyager/http/http_response.h"

namespace voyager {

class Buffer;

class HttpParser {
 public:
  enum HttpType {
    kHttpRequest,
    kHttpResponse
  };

  explicit HttpParser(HttpType type = kHttpRequest);
  ~HttpParser();

  bool ParseBuffer(Buffer* buf);
  bool FinishParse() const { return state_ == kBody; }

  HttpRequest& GetRequest() { return request_; }
  HttpResponse& GetResponse() { return response_; }

 private:
  enum ParserState {
    kLine,
    kHeaders,
    kBody,
  };

  bool ParseRequestLine(const char* begin, const char* end);
  bool ParseResponseLine(const char* begin, const char* end);

  HttpType type_;
  ParserState state_;

  union {
    HttpRequest request_;
    HttpResponse response_;
  };

  // No Copying allow
  HttpParser(const HttpParser&);
  void operator=(const HttpParser&);
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_PARSER_H_
