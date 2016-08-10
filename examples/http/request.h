#ifndef VOYAGER_EXAMPLES_HTTP_REQUEST_H_
#define VOYAGER_EXAMPLES_HTTP_REQUEST_H_

#include <map>
#include <utility>
#include <strings.h>
#include "voyager/util/logging.h"
namespace http {

class Request {
 public:
  enum Method {
    kGet = 0,
    kPost = 1,
    kHead = 2,
    kPut = 3,
    kDelete = 4,
    kNotSupported = 5
  };

  enum Version {
    kHttp10 = 0,
    kHttp11 = 1,
    kUnknown = 2
  };

  Request()
      : state_(0),
        method_(kNotSupported),
        version_(kUnknown) {
  }

  ~Request() {
  }

  void set_state(int s) { state_ = s; }
  int state() const { return state_; }

  bool set_method(const char* buf, size_t size) {
    if (strncasecmp(buf, "GET", size) == 0) {
      method_ = kGet;
    } else if (strncasecmp(buf, "POST", size) == 0) {
      method_ = kPost;
    } else if (strncasecmp(buf, "HEAD", size) == 0) {
      method_ = kHead;
    } else if (strncasecmp(buf, "PUT", size) == 0) {
      method_ = kPut;
    } else if (strncasecmp(buf, "DELETE", size) == 0) {
      method_ = kDelete;
    } else {
      method_ = kNotSupported;
    }
    return method_ != kNotSupported;
  }

  Method method() {
    return method_;
  }

  std::string MethodToString() const;

  void set_version(Version version) {
    version_ = version; 
  }

  Version version() const {
    return version_;
  }

  std::string VersionToString() const;

  void set_query_string(const std::string& s) {
    query_string_ = s;
  }

  void set_query_string(std::string&& s) {
    query_string_ = std::move(s);
  }

  std::string query_string() const {
    return query_string_;
  }

  void set_path(const std::string& s) {
    path_ = s;
  }

  void set_path(std::string&& s) {
    path_ = std::move(s);
  }

  std::string path() const {
    return path_;
  }

  void AddHeader(const char* begin, const char* colon, const char* end) {
    std::string field(begin, colon);
    ++colon;
    while (colon < end && isspace(*colon)) {
      ++colon;
    }
    const char* tmp = end;
    --tmp;
    while (isspace(*tmp)) {
      --tmp;
    }
    ++tmp;
    std::string value(colon, tmp);
    headers_.insert(std::make_pair(field, value));
  }

  std::string GetHeader(const std::string& field) const {
    std::string value;
    std::map<std::string, std::string>::const_iterator it = headers_.find(field);
    if (it != headers_.end()) {
      value = it->second;
    }
    return value;
  }

  const std::map<std::string, std::string>& headers() const {
    return headers_;
  }

  void Reset() {
    state_ = 0;
    method_ = kNotSupported;
    version_ = kUnknown;
    query_string_ = "";
    path_ = "";
    headers_.clear();
  }

 private:
  int state_;
  Method method_;
  Version version_;
  std::string query_string_;
  std::string path_;
  std::map<std::string, std::string> headers_;
};

}  // namespace http

#endif  // VOYAGER_EXAMPLES_HTTP_REQUEST_H_
