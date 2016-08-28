#include "voyager/http/http_client.h"
#include "voyager/core/eventloop.h"

#include <iostream>
#include <functional>

namespace voyager {

void HandleResponse(HttpResponse* response) {
  std::cout << response->ResponseMessage().RetrieveAllAsString();
}

}  // namespace voyager

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Usage: httpclient <host>" << std::endl;
    return 1;
  }

  const char* host =  argv[1];

  voyager::EventLoop ev;
  voyager::HttpClient client(&ev);
  client.SetRequestCallback(std::bind(voyager::HandleResponse,
                                      std::placeholders::_1));
  voyager::HttpRequest request;
  request.SetMethod(voyager::HttpRequest::kGet);
  request.SetPath(host);
  request.SetVersion(voyager::HttpMessage::kHttp11);
  request.AddHeader("Connection", "close");

  client.DoHttpRequest(&request);
  ev.Loop();
}
