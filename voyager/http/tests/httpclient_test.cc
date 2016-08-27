#include "voyager/http/http_client.h"
#include "voyager/core/eventloop.h"

#include <iostream>
#include <functional>

namespace voyager {

void HandleResponse(HttpResponse* response) {
  std::cout << response->ResponseMessage().RetrieveAllAsString();
}

}  // namespace voyager

int main() {
  voyager::EventLoop ev;
  voyager::HttpClient client(&ev);
  client.SetRequestCallback(std::bind(voyager::HandleResponse,
                                      std::placeholders::_1));
  voyager::HttpRequest request;
  request.SetPath("127.0.0.1:5666/");
  request.SetMethod(voyager::HttpRequest::kGet);
  client.DoHttpRequest(&request);
  ev.Loop();
}
