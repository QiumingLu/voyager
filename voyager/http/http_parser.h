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

  bool ParseBuffer(Buffer* buf);
  bool FinishParse() const { return state_ == kEnd; }

  HttpRequestPtr GetRequest() const { return request_; }
  HttpResponsePtr GetResponse() const { return response_; }

  void Reset();

 private:
  enum ParserState {
    kLine,
    kHeaders,
    kBody,
    kEnd
  };

  bool ParseRequestLine(const char* begin, const char* end);
  bool ParseResponseLine(const char* begin, const char* end);
  bool ParseBody(Buffer* buf);

  HttpType type_;
  ParserState state_;
  HttpRequestPtr request_;
  HttpResponsePtr response_;

  // No copying allowed
  HttpParser(const HttpParser&);
  void operator=(const HttpParser&);
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_PARSER_H_
