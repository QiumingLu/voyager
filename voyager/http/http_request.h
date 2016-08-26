#ifndef VOYAGER_HTTP_HTTP_REQUEST_H_
#define VOYAGER_HTTP_HTTP_REQUEST_H_

#include <string>

#include "voyager/http/http_message.h"

namespace voyager {

class HttpRequest : public HttpMessage {
 public:
  enum Method {
    kOptions = 0,
    kHead    = 1,
    kGet     = 2,
    kPost    = 3,
    kPut     = 4,
    kDelete  = 5,
    kTrace   = 6,
    kConnect = 7,
    kPatch   = 8,
    kUnknown = 9
  };

  void SetMethod(Method method) { method_ = method; }
  Method GetMethod() const { return method_; }

  void SetUri(const std::string& uri) { uri_ = uri; }
  const std::string& GetUri() const { return uri_; }

  void SetPath(const std::string& path) { path_ = path; }
  const std::string& GetPath() const { return path_; }

  void SetQuery(const std::string& query) { query_ = query; }
  const std::string& Query() const { return query_; }

 private:
  Method method_;
  std::string uri_;
  std::string path_;
  std::string query_;
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_REQUEST_H_
