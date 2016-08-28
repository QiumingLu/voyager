#include "voyager/http/http_client.h"
#include "voyager/core/eventloop.h"

#include <iostream>
#include <functional>

namespace voyager {

void HandleResponse(HttpResponsePtr response) {
  std::cout << response->ResponseMessage().RetrieveAllAsString() << std::endl;
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
  client.SetRequestCallback(std::bind(voyager::HandleResponse,
                                      std::placeholders::_1));
  voyager::HttpRequestPtr request(new voyager::HttpRequest());
  request->SetMethod(voyager::HttpRequest::kGet);
  request->SetPath(path);
  request->SetVersion(voyager::HttpMessage::kHttp11);
  request->AddHeader("Host", host);
  request->AddHeader("Connection", "keep-alive");

  client.DoHttpRequest(request);
  ev.Loop();
}
