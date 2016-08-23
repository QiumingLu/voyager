#include "voyager/util/json/json.h"

#include <math.h>

#include <cctype>
#include <map>
#include <sstream>
#include <string>
#include <iomanip>

namespace voyager {
namespace json {

Status Json::Stringify(const JsonValuePtr& p, std::string* result) {
  if (!p.get()) {
    result->append("null");
    return Status::OK();
  }
  switch (p->GetType()) {
    case kJsonTypeString:
      return WriteString(p, result);
    case kJsontypeNum:
      return WriteNum(p, result);
    case kJsonTypeObj:
      return WriteObj(p, result);
    case kJsonTypeArray:
      return WriteArray(p, result);
    case kJsonTypeBoolean:
      return WriteBoolean(p, result);
    default:
      Slice info("Invalid JsonType");
      return Status::InvalidArgument(info);
  }
}

Status Json::Parse(const std::string& str, JsonValuePtr* p) {
  JsonReader reader;
  reader.SetBuffer(str.c_str(), str.length());
  return GetValue(&reader, p);
}

Status Json::WriteString(const std::string& s, std::string* result) {
  size_t i, begin = 0;
  std::string replace;
  result->append("\"");
  for (i = 0; i < s.size(); ++i) {
    if (s[i] == '"') {
      replace = "\\\"";
    } else if (s[i] == '\\') {
      replace = "\\\\";
    } else if (s[i] == '/') {
      replace = "\\/";
    } else if (s[i] == '\b') {
      replace = "\\b";
    } else if (s[i] == '\f') {
      replace = "\\f";
    } else if (s[i] == '\n') {
      replace = "\\n";
    } else if (s[i] == '\r') {
      replace = "\\r";
    } else if (s[i] == '\t') {
      replace = "\\t";
    } else if ((unsigned char)s[i] < 0x20) {
      replace = "\\u";
      char buf[16];
      snprintf(buf, sizeof(buf), "%04x", (unsigned char)s[i]);
      replace.append(std::string(buf, 4));
    } else {
      continue;
    }
    result->append(s.substr(begin, i - begin));
    result->append(replace);
    begin = i + 1;
  }
  result->append(s.substr(begin, i - begin));
  result->append("\"");
  return Status::OK();
}


Status Json::WriteString(const JsonValuePtr& p, std::string* result) {
  JsonValueString* str_p = dynamic_cast<JsonValueString*>(p.get());
  const std::string& s = str_p->Value();
  return WriteString(s, result);
}

Status Json::WriteNum(const JsonValuePtr& p, std::string* result) {
  std::ostringstream s;
  JsonValueNum* num_p = dynamic_cast<JsonValueNum*>(p.get());
  if (num_p->IsInt()) {
    s << std::setprecision(30) << num_p->Value();
  } else {
    s << num_p->Value();
  }
  result->append(s.str());
  return Status::OK();
}

Status Json::WriteObj(const JsonValuePtr& p, std::string* result) {
  result->append("{ ");
  JsonValueObj* obj_p = dynamic_cast<JsonValueObj*>(p.get());
  const std::map<std::string, JsonValuePtr>& m = obj_p->Value();
  for (std::map<std::string, JsonValuePtr>::const_iterator it = m.begin();
       it != m.end(); ++it) {
    if (it != m.begin()) {
      result->append(", ");
    }
    WriteString(it->first, result);
    result->append(": ");
    Stringify(it->second, result);
  }
  result->append(" }");
  return Status::OK();
}

Status Json:: WriteArray(const JsonValuePtr& p, std::string* result) {
  result->append("[ ");
  JsonValueArray* array_p = dynamic_cast<JsonValueArray*>(p.get());
  for (size_t i = 0; i < array_p->Value().size(); ++i) {
    if (i != 0) {
      result->append(", ");
    }
    std::string temp;
    Stringify(array_p->Value()[i], &temp);
    result->append(temp);
  }
  result->append(" ]");

  return Status::OK();
}

Status Json::WriteBoolean(const JsonValuePtr& p, std::string* result) {
  JsonValueBoolean* bool_p = dynamic_cast<JsonValueBoolean*>(p.get());
  if (bool_p->Value()) {
    result->append("true");
  } else {
    result->append("false");
  }
  return Status::OK();
}

Status Json::GetValue(JsonReader* reader, JsonValuePtr* p) {
  char c = SkipWhiteSpace(reader);
  switch (c) {
    case '{':
      return GetObj(reader, p);

    case '[':
      return GetArray(reader, p);

    case '"':
      return GetString(reader, c, p);

    case 'T':
    case 't':
    case 'F':
    case 'f':
      return GetBoolean(reader, c, p);

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '-':
      return GetNum(reader, c, p);

    case 'n':
    case 'N':
      return GetNull(reader, c, p);

    default:
      break;
  }
  char s[64];
  snprintf(s, sizeof(s),
           "buffer overflow when peekBuf, over %u.",
           (uint32_t)(uint32_t)reader->Current());
  Slice info(s);
  return Status::InvalidArgument(info);
}

Status Json::GetString(JsonReader* reader, char head, JsonValuePtr* p) {
  JsonValueString* str_p = new JsonValueString();
  const char* peek = reader->Peek();
  char c = '\0';
  uint32_t i = 0;
  while (reader->HasEnd() == false) {
    c = reader->Read();
    if (c == '\\') {
      str_p->value_.append(peek, i);
      peek = peek + i + 2;
      i = 0;
      if (reader->HasEnd()) {
        break;
      }
      c = reader->Read();
      if (c == '\\' || c == '\"' || c == '/') {
        str_p->value_.append(1, c);
      } else if (c == 'b') {
        str_p->value_.append(1, '\b');
      } else if (c == 'f') {
        str_p->value_.append(1, '\f');
      } else if (c == 'n') {
        str_p->value_.append(1, '\n');
      } else if (c == 'r') {
        str_p->value_.append(1, '\r');
      } else if (c == 't') {
        str_p->value_.append(1, '\t');
      } else if (c == 'u') {
        uint16_t icode;
        if (!GetHex(reader, &icode).ok() || icode > 0xff) {
          char s[64];
          snprintf(s, sizeof(s),
                   "get string error(\\u)[pos:%u]",
                   (uint32_t)reader->Current());
          Slice info(s);
          return Status::InvalidArgument(info);
        }
        peek += 4;
        str_p->value_.append(1, static_cast<char>(icode));
      }

    } else if (c == head) {
      break;
    } else {
      ++i;
    }
  }

  if (c != head) {
    char s[64];
    snprintf(s, sizeof(s),
             "get string error(\\u)[pos:%u]",
             (uint32_t)reader->Current());
    Slice info(s);
    return Status::InvalidArgument(info);
  }

  str_p->value_.append(peek, i);
  p->reset(str_p);

  return Status::OK();
}

Status Json::GetNum(JsonReader* reader, char head, JsonValuePtr* p) {
  bool is_ok = true;
  bool is_float = false;
  bool is_exponential = false;
  bool is_negative = false;
  bool is_exponential_negative = false;
  int64_t int_value = 0;
  double float_value = 0;
  double float_rat_value = 0;
  int64_t exponential_value = 0;

  if (head == '-') {
    is_ok = false;
    is_negative = true;
  } else {
    int_value = head-0x30;
  }

  char c;
  bool is_needback = false;
  while (1) {
    if (reader->HasEnd()) {
      break;
    }
    c = reader->Read();
    if (c >= 0x30 && c <= 0x39) {
      is_ok = true;
      if (is_exponential) {
        exponential_value = exponential_value * 10 + c - 0x30;
      } else if (is_float) {
        float_value = float_value + float_rat_value * (c - 0x30);
        float_rat_value = float_rat_value * 0.1;
      } else {
        int_value = int_value * 10 + c - 0x30;
      }
    } else if (c == '.' && !is_float && !is_exponential && is_ok) {
      is_ok = false;
      is_float = true;
      float_rat_value = 0.1;
    } else if ((c == 'e' || c == 'E') && !is_exponential && is_ok) {
      is_ok = false;
      is_exponential = true;
      exponential_value = 0;
      if (reader->HasEnd()) {
        break;
      }
      c = reader->Read();
      if (c == '-') {
        is_exponential_negative = true;
      } else if (c == '+') {
        is_exponential_negative = false;
      } else if (c >= 0x30 && c <= 0x39) {
        is_ok = true;
        exponential_value = c - 0x30;
      } else {
        is_needback = true;
        break;
      }
    } else {
      is_needback = true;
      break;
    }
  }

  if (!is_ok) {
    char s[64];
    snprintf(s, sizeof(s),
             "get num error[pos:%u]", (uint32_t)reader->Current());
    Slice info(s);
    return Status::InvalidArgument(info);
  }

  if (is_needback) {
    reader->Back();
  }
  if (is_exponential_negative) {
    exponential_value = 0 - exponential_value;
  }

  double result = (static_cast<double>(int_value)+ float_value) *
                      pow(static_cast<double>(10),
                           static_cast<double>(exponential_value));
  if (is_negative) {
    result = 0 - result;
  }

  JsonValueNum* num_p = new JsonValueNum();
  num_p->value_ = result;

  return Status::OK();
}

Status Json::GetObj(JsonReader* reader, JsonValuePtr* p) {
  JsonValueObj* obj_p = new JsonValueObj();
  bool is_first = true;
  Status st;
  while (1) {
    char c = SkipWhiteSpace(reader);
    if (c == '}' && is_first) {
      p->reset(obj_p);
      return Status::OK();
    }
    is_first = false;

    if (c != '"') {
      p->reset();
      char s[64];
      snprintf(s, sizeof(s),
               "get obj error(key is not string)[pos:%u]",
               (uint32_t)reader->Current());
      Slice info(s);
      return Status::InvalidArgument(info);
    }

    JsonValuePtr str_ptr;
    st = GetString(reader, '\"', &str_ptr);
    if (!st.ok()) {
      return st;
    }
    JsonValueString* str_p = dynamic_cast<JsonValueString*>(str_ptr.get());

    c = SkipWhiteSpace(reader);
    if (c != ':') {
      p->reset();
      char s[64];
      snprintf(s, sizeof(s),
               "get obj error(: not find)[pos:%u]",
               (uint32_t)reader->Current());
      Slice info(s);
      return Status::InvalidArgument(info);
    }

    JsonValuePtr temp;
    st = GetValue(reader, &temp);
    if (!st.ok()) {
      return st;
    }
    obj_p->value_[str_p->value_] = temp;

    c = SkipWhiteSpace(reader);

    if (c == ',') {
      continue;
    }

    if (c == '}') {
      p->reset(obj_p);
      return Status::OK();
    }

    char s[64];
    snprintf(s, sizeof(s),
             "get obj error(, not find)[pos:%u]",
             (uint32_t)reader->Current());
    Slice info(s);
    return Status::InvalidArgument(info);
  }
}

Status Json::GetArray(JsonReader* reader, JsonValuePtr* p) {
  JsonValueArray* array_p = new JsonValueArray();
  bool is_first = true;
  char c = SkipWhiteSpace(reader);

  while (!reader->HasEnd()) {
    if (is_first) {
      if (c == ']') {
        p->reset(array_p);
        return Status::OK();
      }
      reader->Back();
    }
    is_first = false;

    JsonValuePtr value_ptr;
    Status st = GetValue(reader, &value_ptr);
    if (!st.ok()) {
      return st;
    }

    array_p->push_back(value_ptr);

    c = SkipWhiteSpace(reader);
    if (c == ',') {
      continue;
    }

    if (c == ']') {
      p->reset(array_p);
      return Status::OK();
    }

    char s[64];
    snprintf(s, sizeof(s),
             "get vector error(, not find )[pos:%u]",
             (uint32_t)reader->Current());
    Slice info(s);
    return Status::InvalidArgument(info);
  }

  char s[64];
  snprintf(s, sizeof(s),
           "get vector error(] not find )[pos:%u]",
           (uint32_t)reader->Current());
  Slice info(s);
  return Status::InvalidArgument(info);
}

Status Json::GetBoolean(JsonReader* reader, char c, JsonValuePtr* p) {
  bool is_ok = false;
  bool value = false;
  if (c == 't' || c == 'T') {
    if (!strncasecmp(reader->Peek(), "true", 4)) {
      value = true;
      is_ok = true;
      for (int i = 1; i < 4; ++i) {
        reader->Read();
      }
    }
  } else if (c == 'f' || c == 'F') {
    if (!strncasecmp(reader->Peek(), "false", 5)) {
      value = false;
      is_ok = true;
      for (int i = 1; i < 5; ++i) {
        reader->Read();
      }
    }
  }

  if (!is_ok) {
    char s[64];
    snprintf(s, sizeof(s),
             "get bool error[pos:%u]",
             (uint32_t)reader->Current());
    Slice info(s);
    return Status::InvalidArgument(info);
  }

  JsonValueBoolean* bool_p = new JsonValueBoolean();
  bool_p->value_ = value;
  p->reset(bool_p);
  return Status::OK();
}

Status Json::GetNull(JsonReader* reader, char c, JsonValuePtr* /* p */) {
  assert(c == 'n' || c == 'N');
  bool is_ok = false;
  if (!strncasecmp(reader->Peek(), "null", 4)) {
    is_ok = true;
    for (int i = 1; i < 4; ++i) {
      reader->Read();
    }
  }
  if (!is_ok) {
    char s[64];
    snprintf(s, sizeof(s),
             "get NULL error[pos:%u]",
             (uint32_t)reader->Current());
    Slice info(s);
    return Status::InvalidArgument(info);
  }
  return Status::OK();
}

Status Json::GetHex(JsonReader* reader, uint16_t* result) {
  uint16_t code = 0;
  char c;
  for (int i = 0; i < 4; ++i) {
    c = reader->Read();
    if (c >= 'a' && c <= 'f') {
      code = static_cast<uint16_t>(code * 16 + c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
      code = static_cast<uint16_t>(code * 16 + c - 'A' + 10);
    } else if (c >= '0' && c <= '9') {
      code = static_cast<uint16_t>(code * 16 + c - '0');
    } else {
      char s[64];
      snprintf(s, sizeof(s), "get string error(\\u)[pos:%u]",
               (uint32_t)reader->Current());
      Slice info(s);
      Status::InvalidArgument(info);
    }
  }
  *result = code;
  return Status::OK();
}

char Json::SkipWhiteSpace(JsonReader* reader) {
  char c = ' ';
  while (!reader->HasEnd() &&
         (c == ' ' || c == '\t' || c == '\r' || c == '\n'))  {
    c = reader->Read();
  }
  return c;
}

}  // namespace json
}  // namespace voyager
