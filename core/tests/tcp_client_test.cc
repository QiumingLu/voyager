#include "core/tcp_client.h"
#include "core/buffer.h"
#include "core/callback.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"
#include "core/tcp_connection.h"
#include "util/logging.h"
#include "util/stringprintf.h"
#include "util/slice.h"

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
  mirants::SockAddr serveraddr("127.0.0.1", 5666);
  g_client = new mirants::TcpClient("test", &ev, serveraddr);
  g_client->SetMessageCallback(
      std::bind(mirants::Message, _1, _2));
  g_client->Connect();
  ev.RunAfter(30, std::bind(mirants::DeleteClient));
  ev.RunAfter(40, std::bind(&mirants::EventLoop::Exit, &ev));
  ev.Loop();
  return 0;
}
