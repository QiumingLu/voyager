#include "voyager/http/http_parse.h"

namespace voyager {

void HttpParse::ParseRequest(Buffer* buf, std::shared_ptr<HttpParse>* request) {
  bool ok = true;
  bool flag = true;
  while (flag) {
    if (request->state() == 0) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        ok = ProcessRequestLine(buf->Peek(), crlf, request);
        if (ok) {
          buf->RetrieveUntil(crlf+2);
          request->set_state(1);
        } else {
          flag = false;
        }
      } else {
        flag = false;
      }
    } else if (request->state() == 1) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        const char* colon = buf->Peek();
        while (colon != crlf && *colon != ':') {
          ++colon;
        }
        if (colon != crlf) {
          request->AddHeader(buf->Peek(), colon, crlf);
        } else {
          request->set_state(2);
          flag = false;
        }
        buf->RetrieveUntil(crlf + 2);
      } else {
        flag = false;
      }
    }
  }

  return ok;
}

bool HttpParse::ProcessRequestLine(const char* begin,
                                    const char* end,
                                    Request* request) {
  bool ok = false;
  const char* tmp = begin;
  size_t size = 0;
  while (tmp != end && !isspace(*tmp)) {
    ++size;
    ++tmp;
  }
  if (tmp != end && request->set_method(begin, size)) {
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
        std::string path(start, p);
        request->set_path(path);
        std::string query(p, tmp);
        request->set_query_string(query);
      } else {
        std::string path(start, tmp);
        request->set_path(path);
      }
      start = tmp + 1;
      ok = end-start == 8 && std::equal(start, end-1, "HTTP/1.");
      if (ok) {
        if (*(end-1) == '1') {
          request->set_version(Request::kHttp11);
        } else if (*(end -1) == '0') {
          request->set_version(Request::kHttp10);
        } else {
          ok = false;
        }
      }
    }
  }

  return ok;
}

}

}  // namespace voyager
