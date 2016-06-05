#include "examples/http/httpserver.h"
#include "examples/http/request.h"
#include "examples/http/response.h"
#include "mirants/util/logging.h"

namespace http {

using namespace std::placeholders;

HttpServer::HttpServer(mirants::EventLoop* ev,
                       const mirants::SockAddr& addr,
                       const std::string& name,
                       int thread_size)
  : server_(ev, addr, name, thread_size) {
  server_.SetConnectionCallback(std::bind(&HttpServer::ConnectCallback, 
                                this, _1));
  server_.SetMessageCallback(std::bind(&HttpServer::MessageCallback,
                             this, _1, _2));  
}

void HttpServer::Start() {
  server_.Start();
}

void HttpServer::ConnectCallback(const mirants::TcpConnectionPtr& ptr) {
}

void HttpServer::MessageCallback(const mirants::TcpConnectionPtr& ptr,
                                  mirants::Buffer* buf) {
  Request request;
  if (!ProcessRequest(buf, &request)) {
    std::string s("HTTP/1.1 400 Bad Request\r\n\r\n");
    ptr->SendMessage(std::move(s));
    ptr->ShutDown();
  } else {
    /*
    MIRANTS_LOG(WARN) << request.MethodToString() << " "
                      << request.path() << request.query_string()
                      << " " << request.VersionToString();
    std::map<std::string, std::string>::const_iterator it;
    for (it = request.headers().begin(); it != request.headers().end(); ++it) {
      MIRANTS_LOG(WARN) << it->first << ":" << it->second;
    }
    */

    std::string s = request.GetHeader("Connection");
    char* tmp = &*(s.begin());
    bool close;
    if (strcasecmp(tmp, "close") == 0 || 
        (request.version() == Request::kHttp10 && strcasecmp(tmp, "keep-alive"))) {
      close = true;
    } else {
      close = false;
    }

    Response response(close);
    
    if (http_cb_) {
      http_cb_(&request, &response);
    } else {
      response.set_status_code(Response::k200);
    }

    mirants::Buffer buffer;
    response.AppendToBuffer(&buffer);
    ptr->SendMessage(&buffer);
    if (response.close_connection()) {
      ptr->ShutDown();
    }    
  }
}


bool HttpServer::ProcessRequest(mirants::Buffer* buf, Request* request) {
  bool ok = true;
  bool flag = true;
  int state = 0;
  while (flag) {
    if (state == 0) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        ok = ProcessRequestLine(buf->Peek(), crlf, request);
        if (ok) {
          buf->RetrieveUntil(crlf+2);
          state = 1;
        } else {
          flag = false;
        }
      } else {
        flag = false;
      }
    } else if (state == 1) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        const char* colon = buf->Peek();
        while (colon != crlf && *colon != ':') {
          ++colon;
        }
        if (colon != crlf) {
          request->AddHeader(buf->Peek(), colon, crlf);
        } else {
          state = 2;
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

bool HttpServer::ProcessRequestLine(const char* begin, 
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
    const char* p = NULL;
    while (tmp != end && !isspace(*tmp)) {
      if (*tmp == '?') {
        p = tmp;
      }
      ++tmp;
    }
    if (tmp != end) {
      if (p != NULL) {
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

} // namespace http
