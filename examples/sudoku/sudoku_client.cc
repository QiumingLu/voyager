#include "mirants/core/tcp_client.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"
#include "mirants/core/tcp_connection.h"
#include "mirants/core/callback.h"
#include "mirants/core/buffer.h"
#include "mirants/util/logging.h"
#include "mirants/port/atomic_sequence_num.h"

using namespace std::placeholders;

int dotimes;

mirants::Timestamp g_start;
mirants::Timestamp g_stop;
mirants::port::SequenceNumber seq;

void Connect(const mirants::TcpConnectionPtr& ptr) {
  MIRANTS_LOG(INFO) << "Start solve sudoku...";
  g_start = mirants::Timestamp::Now();
  std::string message = "53  7    6  195    98    6 8   6   34  8 3  17   2   6 6    28    419  5    8  79\r\n";
  for (int i = 0; i < dotimes; ++i) {
    ptr->SendMessage(std::move(message));
  }
}

void Message(const mirants::TcpConnectionPtr& ptr, mirants::Buffer* buf) {
  if (buf->FindCRLF() != NULL) {
    if (seq.GetNext() == dotimes - 2) {
      g_stop = mirants::Timestamp::Now();
      MIRANTS_LOG(INFO) << "\nStart time is: " << g_start.FormatTimestamp()
                        << "\nFinish time is: " << g_stop.FormatTimestamp()
                        << "\nTake MicroSeconds: " 
                        << g_stop.MicroSecondsSinceEpoch() - g_start.MicroSecondsSinceEpoch();
    }
  }
  std::string s = buf->RetrieveAllAsString();
  (void)s;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: %s server_ip dotimes\n", argv[0]);
    return 0;
  }
  dotimes = atoi(argv[2]);
  mirants::EventLoop ev;
  mirants::SockAddr servaddr(argv[1], 5666);
  mirants::TcpClient client("SudokuClinet", &ev, servaddr);
  client.SetConnectionCallback(std::bind(Connect, _1));
  client.SetMessageCallback(
      std::bind(Message, _1, _2));
  client.Connect();
  ev.Loop();
}
