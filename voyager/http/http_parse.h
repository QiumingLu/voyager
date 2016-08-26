#ifndef VOYAGER_HTTP_HTTP_PARSE_H_
#define VOYAGER_HTTP_HTTP_PARSE_H_

namespace voyager {

class Buffer;
class HttpRequest;

class HttpParse {
 public:
  ParseBuffer(Buffer* buf, std::shared_ptr<HttpRequest>* request);
};

}  // namespace voyager
#endif  // VOYAGER_HTTP_HTTP_PARSE_H_
