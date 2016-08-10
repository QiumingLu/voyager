#ifndef VOYAGER_UTIL_JSON_JSON_VALUE_H_
#define VOYAGER_UTIL_JSON_JSON_VALUE_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace voyager {
namespace json {

enum JsonType {
  kJsonTypeString,
  kJsontypeNum,
  kJsonTypeObj,
  kJsonTypeArray,
  kJsonTypeBoolean
};

class Json;
class JsonValue;
typedef std::shared_ptr<JsonValue> JsonValuePtr;
  
// Json 类型的基类，没有任何意义
class JsonValue : public std::enable_shared_from_this<JsonValue> {
 public:
  JsonValue() { }
  virtual ~JsonValue() { }
        
  virtual JsonType GetType() const = 0;
};
  
class JsonValueString : public JsonValue {
 public:
  JsonValueString() { }
  JsonValueString(const std::string& s) : value_(s) { }
  virtual ~JsonValueString() { }
        
  JsonType GetType() const { return kJsonTypeString; }

  void SetValue(const std::string& s) { value_ = s; }

  void SetValue(std::string&& s) { value_ = std::move(s); }

  const std::string& Value() const { return value_; }
        
 private:
  friend class Json;
  std::string value_;
};
    
class JsonValueNum : public JsonValue {
 public:
  JsonValueNum() : value_(0.0), is_int_(false) { }
  explicit JsonValueNum(double x, bool b = false) : value_(x), is_int_(b) { }
  virtual ~JsonValueNum() { }
        
  JsonType GetType () const { return kJsontypeNum; }

  void SetValue(double d, bool is_int = false) { 
    value_ = d; 
    is_int_ = is_int; 
  }

  double Value() const { return value_; }

  bool IsInt() const { return is_int_; }
        
 private:
  friend class Json;
  double value_;
  bool is_int_;
};
  
class JsonValueObj : public JsonValue {
 public:
  virtual ~JsonValueObj() { }
        
  JsonType GetType() const { return kJsonTypeObj; }
  
  void insert(const std::string& key, JsonValuePtr p) {
    value_[key] = p;
  }

  void SetValue(const std::map<std::string, JsonValuePtr>& m) { value_ = m; }

  void SetValue(std::map<std::string, JsonValuePtr>&& m) { 
    value_ = std::move(m); 
  }

  const std::map<std::string, JsonValuePtr>& Value() const {
    return value_;
  }
        
 private:
  friend class Json;
  std::map<std::string, JsonValuePtr> value_;
};
    
class JsonValueArray : public JsonValue {
 public:
  virtual ~JsonValueArray() { }
        
  JsonType GetType() const { return kJsonTypeArray; }

  void push_back(const JsonValuePtr& v) { value_.push_back(v); }

  void SetValue(const std::vector<JsonValuePtr>& v) { value_ = v; }

  void SetValue(std::vector<JsonValuePtr>&& v) { value_ = std::move(v); }

  const std::vector<JsonValuePtr>& Value() const {
    return value_;
  }
        
 private:
  friend class Json;
  std::vector<JsonValuePtr> value_;
};
    
class JsonValueBoolean : public JsonValue {
 public:
  virtual ~JsonValueBoolean() { }

  JsonType GetType() const { return kJsonTypeBoolean; }

  void SetValue(bool b) { value_ = b; }
  bool Value() const { return value_; }
        
 private:
  friend class Json;
  bool value_;
};

}  // namespace json
}  // namespace voyager

#endif // VOYAGER_UTIL_JSON_JSON_VALUE_H_
