// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/http/http_message.h"

#include <string.h>

namespace voyager {

const char* HttpMessage::kConnection = "Connection";
const char* HttpMessage::kContentLength = "Content-Length";
const char* HttpMessage::kHost = "Host";

bool HttpMessage::SetVersion(const char* begin, const char* end) {
  size_t size = end - begin;
  if (strncasecmp(begin, "HTTP/1.0", size) == 0) {
    version_ = kHttp10;
  } else if (strncasecmp(begin, "HTTP/1.1", size) == 0) {
    version_ = kHttp11;
  } else if (strncasecmp(begin, "HTTP/2", size) == 0) {
    version_ = kHttp20;
  } else {
    return false;
  }
  return true;
}

const char* HttpMessage::VersionToString() const {
  const char* c;
  switch (version_) {
    case kHttp10:
      c = "HTTP/1.0";
      break;
    case kHttp11:
      c = "HTTP/1.1";
      break;
    case kHttp20:
      c = "HTTP/2";
      break;
    default:
      c = "";
      break;
  }
  return c;
}

void HttpMessage::AddHeader(const char* begin, const char* colon,
                            const char* end) {
  std::string field(begin, colon);
  ++colon;
  while (colon != end && *colon == ' ') {
    ++colon;
  }
  std::string value(colon, end);
  AddHeader(field, value);
}

void HttpMessage::AddHeader(const std::string& field,
                            const std::string& value) {
  if (!field.empty() && !value.empty()) {
    std::string key(field);
    TransferField(&key);
    header_map_[key] = value;
  }
}

void HttpMessage::RemoveHeader(const std::string& field) {
  header_map_.erase(field);
}

void HttpMessage::TransferField(std::string* s) {
  const char* begin = &*s->begin();
  if (strcasecmp(begin, kConnection) == 0) {
    s->assign(kConnection);
  } else if (strcasecmp(begin, kContentLength) == 0) {
    s->assign(kContentLength);
  } else if (strcasecmp(begin, kHost) == 0) {
    s->assign(kHost);
  }
}

}  // namespace voyager
