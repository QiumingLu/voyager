#ifndef VOYAGER_HTTP_HTTP_MESSAGE_H_
#define VOYAGER_HTTP_HTTP_MESSAGE_H_

#include <map>
#include <string>

#include "voyager/util/slice.h"

namespace voyager {

class HttpMessage {
 public:
  enum HttpVersion {
    kHttp10 = 0,
    kHttp11 = 1,
    kHttp20 = 2,
    kUnknown = 3
  };

  void SetVersion(HttpVersion version) { version_ = version; }
  HttpVersion GetVersion() const { return version_; }
  std::string VersionToString() const;

  void AppendContent(const Slice& s);
  void SetContent(const std::string& s);
  void SetContent(std::string&& s);
  const std::string& GetContent() const { return content_; }

  void AddHeader(const char* begin, const char* colon, const char* end);
  void AddHeader(const std::string& field, const std::string& value);
  void RemoveHeader(const std::string& field);
  const std::map<std::string, std::string>& HeaderMap() const {
    return header_map_;
  }

 private:
  HttpVersion version_;
  std::string content_;
  std::map<std::string, std::string> header_map_;
};

}  // namespace voyager

#endif  // VOYAGER_HTTP_HTTP_MESSAGE_H_
