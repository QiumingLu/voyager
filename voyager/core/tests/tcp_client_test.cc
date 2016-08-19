#include "voyager/core/tcp_client.h"
#include "voyager/core/buffer.h"
#include "voyager/core/callback.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/util/logging.h"
#include "voyager/util/stringprintf.h"
#include "voyager/util/slice.h"

voyager::TcpClient* g_client = NULL;

namespace voyager {

void OnMessage(const TcpConnectionPtr& p, Buffer* buf) {
  std::string s = buf->RetrieveAllAsString();
  VOYAGER_LOG(INFO)  <<  s;
  if (s == "Nice!") {
    Slice message = "That's OK! I close!";
    p->SendMessage(message);
  } else if (s == "Bye!") {
    VOYAGER_LOG(INFO) << p->StateToString();
    p->ShutDown();
    VOYAGER_LOG(INFO) << p->StateToString();
  } else {   
    Slice message = "Yes, I know!";
    p->SendMessage(message);
  }
}

void DeleteClient() {
  delete g_client;
}

}  // namespace voyager 

using namespace std::placeholders;

int main(int argc, char** argv) {
  voyager::EventLoop ev;
  voyager::SockAddr serveraddr("127.0.0.1", 5666);
  g_client = new voyager::TcpClient(&ev, serveraddr);
  g_client->SetMessageCallback(
      std::bind(voyager::OnMessage, _1, _2));
  g_client->Connect();
  ev.RunAfter(5000000, []() { voyager::DeleteClient(); });
  ev.RunAfter(20000000, [&ev]() { ev.Exit(); });
  ev.Loop();
  return 0;
}
