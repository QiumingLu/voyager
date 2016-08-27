#ifndef VOYAGER_HTTP_HTTP_REQUEST_H_
#define VOYAGER_HTTP_HTTP_REQUEST_H_

#include <string>
#include <utility>

#include "voyager/http/http_message.h"

namespace voyager {

class HttpRequest : public HttpMessage {
 public:
  enum Method {
    kOptions,
    kHead,
    kGet,
    kPost,
    kPut,
    kDelete,
    kTrace,
    kConnect,
    kPatch,
    kUnknown,
  };

  bool SetMethod(const char* begin, const char* end);
  Method GetMethod() const { return method_; }
  const char* MethodToString() const;

  void SetUri(const std::string& uri) { uri_ = uri; }
  const std::string& Uri() const { return uri_; }

  void SetPath(const char* begin, const char* end) {
    path_ = std::string(begin, end);
  }
  const std::string& Path() const { return path_; }

  void SetQuery(const char* begin, const char* end) {
    query_ = std::string(begin, end);
  }
  const std::string& Query() const { return query_; }

 private:
  Method method_;
  std::string uri_;
  std::string path_;
  std::string query_;
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_REQUEST_H_
