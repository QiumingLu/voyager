#include "core/tcp_client.h"
#include "core/buffer.h"
#include "core/callback.h"
#include "core/eventloop.h"
#include "core/sockaddr.h"
#include "core/tcp_connection.h"
#include "util/logging.h"
#include "util/stringprintf.h"
#include "util/slice.h"

namespace mirants {

void Message(const TcpConnectionPtr& p, Buffer* buf) {
  std::string s = buf->RetrieveAllAsString();
  MIRANTS_LOG(INFO)  <<  s;
  if (s == "Nice!") {
    Slice message = "That's OK! I close!";
    p->SendMessage(message);
  } else if (s == "Bye!") {
    p->ShutDown();
  } else {   
    Slice message = "Yes, I know!";
    p->SendMessage(message);
  }
}

}  // namespace mirants 

using namespace std::placeholders;

int main(int argc, char** argv) {
  mirants::EventLoop ev;
  mirants::SockAddr serveraddr("127.0.0.1", 5666);
  mirants::TcpClient client("test", &ev, serveraddr);
  client.SetMessageCallback(
      std::bind(mirants::Message, _1, _2));
  client.Connect();
  ev.Loop();
  return 0;
}
