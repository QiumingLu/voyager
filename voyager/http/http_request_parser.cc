#include "voyager/http/http_request_parser.h"

#include <stdio.h>

#include "voyager/core/buffer.h"

namespace voyager {

HttpRequestParser::HttpRequestParser()
    : state_(kLine), request_(new HttpRequest()) {
}

bool HttpRequestParser::ParseBuffer(Buffer* buf) {
  bool ok = true;
  bool flag = true;
  while (flag && ok) {
    if (state_ == kLine) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        ok = ParseRequestLine(buf->Peek(), crlf);
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
            request_->AddHeader(buf->Peek(), colon, crlf);
          }
        }
        buf->RetrieveUntil(crlf + 2);
      } else {
        flag = false;
      }
    } else if (state_ == kBody) {
      if (ParseRequestBody(buf)) {
        state_ = kEnd;
      }
      flag = false;
    } else {
    }
  }
  return ok;
}

void HttpRequestParser::Reset() {
  request_.reset(new HttpRequest());
}

bool HttpRequestParser::ParseRequestLine(const char* begin, const char* end) {
  bool ok = false;
  const char* tmp = begin;
  while (tmp != end && !isspace(*tmp)) {
    ++tmp;
  }
  if (tmp != end && request_->SetMethod(begin, tmp)) {
    ++tmp;
    const char* start = tmp;
    const char* p = nullptr;
    while (tmp != end && !isspace(*tmp)) {
      if (*tmp == '?') {
        p = tmp;
      }
      ++tmp;
    }
    if (tmp != end) {
      if (p != nullptr) {
        request_->SetPath(start, p);
        request_->SetQuery(p, tmp);
      } else {
        request_->SetPath(start, tmp);
      }
      start = tmp + 1;
      ok = request_->SetVersion(start, end);
    }
  }
  return ok;
}

bool HttpRequestParser::ParseRequestBody(Buffer* buf) {
  const std::string& s = request_->Value(HttpMessage::kContentLength);
  if (request_->GetMethod() != HttpRequest::kPost || s.empty()) {
    return true;
  }
  if (static_cast<int>(buf->ReadableSize()) == atoi(&*(s.begin()))) {
    request_->SetBody(buf->RetrieveAllAsString());
    return true;
  }
  return false;
}

}  // namespace voyager
