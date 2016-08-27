#ifndef VOYAGER_HTTP_HTTP_PARSER_H_
#define VOYAGER_HTTP_HTTP_PARSER_H_

namespace voyager {

class Buffer;
class HttpRequest;

class HttpParser {
 public:
  HttpParser();
  ~HttpParser();

  bool ParseBuffer(Buffer* buf);
  bool FinishParse() const { return state_ == kRequestBody; }
  HttpRequest* GetRequest() const { return request_; }

 private:
  enum ParserState {
    kRequestLine,
    kRequestHeader,
    kRequestBody,
  };

  bool ParseRequestLine(const char* begin, const char* end);

  ParserState state_;
  HttpRequest* request_;

  // No Copying allow
  HttpParser(const HttpParser&);
  void operator=(const HttpParser&);
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_PARSER_H_
