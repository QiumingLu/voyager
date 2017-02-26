// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/http/http_response_parser.h"

#include <stdio.h>
#include <string>

#include "voyager/core/buffer.h"

namespace voyager {

HttpResponseParser::HttpResponseParser()
    : state_(kLine), response_(new HttpResponse()) {
}

bool HttpResponseParser::ParseBuffer(Buffer* buf) {
  bool ok = true;
  bool flag = true;
  while (flag && ok) {
    if (state_ == kLine) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        ok = ParseResponseLine(buf->Peek(), crlf);
        if (ok) {
          buf->RetrieveUntil(crlf+2);
          state_ = kHeaders;
        } else {
          flag = false;
        }
      } else {
        flag = false;
      }
    } else if (state_ == kHeaders) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        const char* colon = buf->Peek();
        if (colon == crlf) {
          state_ = kBody;
        } else {
          while (colon != crlf && *colon != ':') {
            ++colon;
          }
          if (colon == buf->Peek() || colon == crlf) {
            ok = false;
          } else {
            response_->AddHeader(buf->Peek(), colon, crlf);
          }
        }
        buf->RetrieveUntil(crlf + 2);
      } else {
        flag = false;
      }
    } else if (state_ == kBody) {
      if (ParseResponseBody(buf)) {
        state_ = kEnd;
      } else {
        ok = false;
      }
      flag = false;
    } else {
    }
  }
  return ok;
}

void HttpResponseParser::Reset() {
  state_ = kLine;
  response_.reset(new HttpResponse());
}

bool HttpResponseParser::ParseResponseLine(const char* begin, const char* end) {
  const char* tmp = begin;
  while (tmp != end && !isspace(*tmp)) {
    ++tmp;
  }
  response_->SetVersion(begin, tmp);

  while (tmp != end && isspace(*tmp)) {
    ++tmp;
  }

  begin = tmp;
  while (tmp != end && !isspace(*tmp)) {
    ++tmp;
  }
  response_->SetStatusCode(begin, tmp);

  while (tmp != end && isspace(*tmp)) {
    ++tmp;
  }
  response_->SetReasonParse(tmp, end);
  return true;
}

bool HttpResponseParser::ParseResponseBody(Buffer* buf) {
  const std::string& s = response_->Value(HttpMessage::kContentLength);
  if (s.empty()) {
    return true;
  }
  if (static_cast<int>(buf->ReadableSize()) == atoi(&*(s.begin()))) {
    response_->SetBody(buf->RetrieveAllAsString());
    return true;
  }
  return false;
}

}  // namespace voyager
