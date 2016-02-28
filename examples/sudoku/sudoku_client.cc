#include "mirants/core/tcp_client.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"
#include "mirants/core/tcp_connection.h"
#include "mirants/core/callback.h"
#include "mirants/core/buffer.h"
#include "mirants/util/logging.h"

using namespace std::placeholders;
void Connect(const mirants::TcpConnectionPtr& ptr) {
  std::string message = "53  7    6  195    98    6 8   6   34  8 3  17   2   6 6    28    419  5    8  79\r\n";
  for (int i = 0; i < 10000; ++i) {
    ptr->SendMessage(std::move(message));
  }
}

void Message(const mirants::TcpConnectionPtr& ptr, mirants::Buffer* buf) {
  std::string s = buf->RetrieveAllAsString();
  MIRANTS_LOG(INFO) << s;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("Usage: %s server_ip\n", argv[0]);
    return 0;
  }
  mirants::EventLoop ev;
  mirants::SockAddr servaddr(argv[1], 5666);
  mirants::TcpClient client("SudokuClinet", &ev, servaddr);
  client.SetConnectionCallback(std::bind(Connect, _1));
  client.SetMessageCallback(
      std::bind(Message, _1, _2));
  client.Connect();
  ev.Loop();
}