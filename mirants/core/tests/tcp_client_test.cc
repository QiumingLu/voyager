#include "mirants/core/tcp_client.h"
#include "mirants/core/buffer.h"
#include "mirants/core/callback.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"
#include "mirants/core/tcp_connection.h"
#include "mirants/util/logging.h"
#include "mirants/util/stringprintf.h"
#include "mirants/util/slice.h"

mirants::TcpClient* g_client = NULL;

namespace mirants {

void Message(const TcpConnectionPtr& p, Buffer* buf) {
  std::string s = buf->RetrieveAllAsString();
  MIRANTS_LOG(INFO)  <<  s;
  if (s == "Nice!") {
    Slice message = "That's OK! I close!";
    p->SendMessage(message);
  } else if (s == "Bye!") {
    MIRANTS_LOG(INFO) << p->StateToString();
    g_client->DisConnect();
    MIRANTS_LOG(INFO) << p->StateToString();
  } else {   
    Slice message = "Yes, I know!";
    p->SendMessage(message);
  }
}

void DeleteClient() {
  delete g_client;
}

}  // namespace mirants 

using namespace std::placeholders;

int main(int argc, char** argv) {
  mirants::EventLoop ev;
  mirants::SockAddr serveraddr("172.31.44.121", 5666);
  g_client = new mirants::TcpClient("test", &ev, serveraddr);
  g_client->SetMessageCallback(
      std::bind(mirants::Message, _1, _2));
  g_client->Connect();
  ev.RunAfter(30, std::bind(mirants::DeleteClient));
  ev.RunAfter(40, std::bind(&mirants::EventLoop::Exit, &ev));
  ev.Loop();
  return 0;
}
