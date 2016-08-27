#include "voyager/http/http_parser.h"
#include "voyager/http/http_request.h"
#include "voyager/core/buffer.h"

namespace voyager {

HttpParser::HttpParser()
    : state_(kRequestLine), request_(new HttpRequest()) {
}

HttpParser::~HttpParser() {
  delete request_;
}

bool HttpParser::ParseBuffer(Buffer* buf) {
  bool ok = true;
  bool flag = true;
  while (flag) {
    if (state_ == kRequestLine) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        ok = ParseRequestLine(buf->Peek(), crlf);
        if (ok) {
          buf->RetrieveUntil(crlf+2);
          state_ = kRequestHeader;
        } else {
          flag = false;
        }
      } else {
        flag = false;
      }
    } else if (state_ == kRequestHeader) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        const char* colon = buf->Peek();
        while (colon != crlf && *colon != ':') {
          ++colon;
        }
        if (colon != crlf) {
          request_->AddHeader(buf->Peek(), colon, crlf);
        } else {
          state_ = kRequestBody;
        }
        buf->RetrieveUntil(crlf + 2);
      } else {
        flag = false;
      }
    } else if (state_ == kRequestBody) {
      request_->SetBody(buf->RetrieveAllAsString());
      flag = false;
    }
  }

  return ok;
}

bool HttpParser::ParseRequestLine(const char* begin, const char* end) {
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

}  // namespace voyager
