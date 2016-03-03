#include "mirants/core/tcp_client.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/sockaddr.h"
#include "mirants/core/tcp_connection.h"
#include "mirants/core/callback.h"
#include "mirants/core/buffer.h"
#include "mirants/util/logging.h"
#include <vector>

using namespace std::placeholders;

namespace sudoku {

class SudokuClient {
 public:
  SudokuClient(const std::string& name, 
               mirants::EventLoop* ev, 
               const mirants::SockAddr& addr, 
               const std::vector<std::string>& vec)
      : num_(0),
        client_(name, ev, addr),
        vec_(vec) {
    client_.SetConnectionCallback(
        std::bind(&SudokuClient::ConnectCallback,this,  _1));
    client_.SetMessageCallback(
        std::bind(&SudokuClient::MessageCallback, this, _1, _2));
  }

  SudokuClient(const std::string& name, 
               mirants::EventLoop* ev, 
               const mirants::SockAddr& addr, 
               std::vector<std::string>&& vec)
      : num_(0),
        client_(name, ev, addr),
        vec_(std::move(vec)) {
    client_.SetConnectionCallback(
        std::bind(&SudokuClient::ConnectCallback, this, _1));
    client_.SetMessageCallback(
        std::bind(&SudokuClient::MessageCallback, this, _1, _2));
  }

  void Connect() {
    client_.Connect();
  }

 private:
  void ConnectCallback(const mirants::TcpConnectionPtr& ptr) {
    MIRANTS_LOG(INFO) << "Start solve sudoku...";
    start_ = mirants::Timestamp::Now();
    for (size_t i = 0; i < vec_.size(); ++i) {
      ptr->SendMessage(vec_.at(i));
    }
  }

  void MessageCallback(const mirants::TcpConnectionPtr& ptr, mirants::Buffer* buf) {
    size_t size = buf->ReadableSize();
    while (size >= kCells + 2) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        std::string res(buf->Peek(), crlf);
        buf->RetrieveUntil(crlf + 2);
        size = buf->ReadableSize();
        MIRANTS_LOG(INFO) << "The result is: " << res;
        ++num_;
        if (++num_ == static_cast<int64_t>(vec_.size())) {
          stop_ = mirants::Timestamp::Now();
          MIRANTS_LOG(INFO) << "\nStart time is: " << start_.FormatTimestamp()
                            << "\nFinish time is: " << stop_.FormatTimestamp()
                            << "\nTake MicroSeconds: " 
                            << stop_.MicroSecondsSinceEpoch() - start_.MicroSecondsSinceEpoch();
        }
      } else {
        break;
      }
    }
  }

  static const int kCells = 81;
  int64_t num_;
  mirants::TcpClient client_;
  std::vector<std::string> vec_;
  mirants::Timestamp start_;
  mirants::Timestamp stop_;
};

}  // namespace sudoku

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: %s server_ip dotimes\n", argv[0]);
    return 0;
  }
  int dotimes = atoi(argv[2]);
  if (dotimes <= 0) {
    return 0;
  }
  
  std::string message = "53  7    6  195    98    6 8   6   34  8 3  17   2   6 6    28    419  5    8  79\r\n";
  std::vector<std::string> vec(dotimes);
  for (int i = 0; i < dotimes; ++i) {
    vec.push_back(message);
  }
  mirants::EventLoop ev;
  mirants::SockAddr servaddr(argv[1], 5666);
  sudoku::SudokuClient client("SudokuClinet", &ev, servaddr, std::move(vec));
  client.Connect();
  ev.Loop();
}
