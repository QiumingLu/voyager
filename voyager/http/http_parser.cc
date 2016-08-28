#include "voyager/http/http_parser.h"
#include "voyager/http/http_request.h"
#include "voyager/http/http_response.h"
#include "voyager/core/buffer.h"

namespace voyager {

HttpParser::HttpParser(HttpType type)
    : type_(type), state_(kLine), request_(nullptr), response_(nullptr) {
  if (type_ == kHttpRequest) {
    request_ = new HttpRequest();
  } else {
    response_ = new HttpResponse();
  }
}

HttpParser::~HttpParser() {
  delete request_;
  delete response_;
}

bool HttpParser::ParseBuffer(Buffer* buf) {
  bool ok = true;
  bool flag = true;
  while (flag && ok) {
    if (state_ == kLine) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        if (type_ == kHttpRequest) {
          ok = ParseRequestLine(buf->Peek(), crlf);
        } else {
          ok = ParseResponseLine(buf->Peek(), crlf);
        }
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
            if (type_ == kHttpRequest) {
              request_->AddHeader(buf->Peek(), colon, crlf);
            } else  {
              response_->AddHeader(buf->Peek(), colon, crlf);
            }
          }
        }
        buf->RetrieveUntil(crlf + 2);
      } else {
        flag = false;
      }
    } else if (state_ == kBody) {
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

bool HttpParser::ParseResponseLine(const char* begin, const char* end) {
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

}  // namespace voyager
