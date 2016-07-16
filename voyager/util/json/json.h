#ifndef VOYAGER_UTIL_JSON_JSON_H_
#define VOYAGER_UTIL_JSON_JSON_H_

#include "voyager/util/json/json_value.h"
#include "voyager/util/status.h"

#include <assert.h>

namespace voyager {
namespace json {

// 读字符串类
class JsonReader {
 public:
  JsonReader() : data_(""), size_(0), cur_(0) { }
  JsonReader(const char* d, size_t n) : data_(d), size_(n), cur_(0) { }
  JsonReader(const std::vector<char>& v)
      : data_(&v[0]), size_(v.size()), cur_(0) {
  }
  
  void Reset() { cur_ = 0; }
  
  void SetBuffer(const char* d, size_t n) {
    data_ = d;
    size_ = n;
    cur_ = 0;
  }
  
  void SetBuffer(const std::vector<char>& v) {
    data_ = &v[0];
    size_ = v.size();
    cur_ = 0;
  }
  
  size_t Current() const { return cur_; }
  
  const char* Peek() const { return data_ + cur_; }
  
  // 在读之前必须先判断是不是已经到了末尾
  char Read() {
    assert(cur_ + 1 <= size_);
    ++cur_;
    return *(data_ + cur_ - 1);
  }
  
  void Back() {
    assert(cur_ > 0);
    --cur_;
  }
  
  bool HasEnd() const {
    return cur_ >= size_;
  }
  
 private:
  const char* data_;
  size_t size_;
  size_t cur_;
};
 
// 分析Json类
class Json {
 public:
  // Json类型到字符串的转化
  static Status Stringify(const JsonValuePtr& p, std::string* result);
  
  // 字符串到Json类型的转化
  static Status Parse(const std::string& str, JsonValuePtr* p);
  
private:
  static Status WriteString(const std::string& s, std::string* result);
  static Status WriteString(const JsonValuePtr& p, std::string* result);
  static Status WriteNum(const JsonValuePtr& p, std::string* result);
  static Status WriteObj(const JsonValuePtr& p, std::string* result);
  static Status WriteArray(const JsonValuePtr& p, std::string* result);
  static Status WriteBoolean(const JsonValuePtr& p, std::string* result);
  
  static Status GetValue(JsonReader& reader, JsonValuePtr* p);
  static Status GetString(JsonReader& reader, char head, JsonValuePtr* p);
  static Status GetNum(JsonReader& reader, char head, JsonValuePtr* p);
  static Status GetObj(JsonReader& reader, JsonValuePtr* p);
  static Status GetArray(JsonReader& reader, JsonValuePtr* p);
  static Status GetBoolean(JsonReader& reader, char c, JsonValuePtr* p);
  static Status GetNull(JsonReader& reader, char c, JsonValuePtr* /* p */);
 
  static Status GetHex(JsonReader& reader, uint16_t& result);
  static char SkipWhiteSpace(JsonReader& reader);
};
  
}  // namespace json
}  // namespace voyager

#endif  // VOYAGER_UTIL_JSON_JSON_H_
