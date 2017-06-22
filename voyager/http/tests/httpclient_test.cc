// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <functional>
#include <iostream>

#include "voyager/core/eventloop.h"
#include "voyager/http/http_client.h"
#include "voyager/util/status.h"

namespace voyager {

void HandleResponse(HttpResponsePtr response, const Status& s) {
  if (s.ok()) {
    std::cout << response->ResponseMessage().RetrieveAllAsString() << "\n";
  } else {
    std::cout << s.ToString() << "\n";
  }
}

}  // namespace voyager

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: httpclient <host> <path>" << std::endl;
    return 1;
  }

  const char* host = argv[1];
  const char* path = argv[2];

  voyager::EventLoop ev;

  voyager::HttpClient client(&ev);
  voyager::HttpRequestPtr request(new voyager::HttpRequest());
  request->SetMethod(voyager::HttpRequest::kGet);
  request->SetPath(path);
  request->SetVersion(voyager::HttpMessage::kHttp11);
  request->AddHeader("Host", host);
  request->AddHeader("Connection", "keep-alive");

  client.DoHttpRequest(request,
                       std::bind(voyager::HandleResponse, std::placeholders::_1,
                                 std::placeholders::_2));
  ev.Loop();
}
