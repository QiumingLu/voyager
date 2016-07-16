#include "voyager/core/tcp_client.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/core/callback.h"
#include "voyager/core/buffer.h"
#include "voyager/util/logging.h"
#include "voyager/util/stringprintf.h"
#include <vector>

using namespace std::placeholders;

namespace sudoku {

class SudokuClient {
 public:
  SudokuClient(voyager::EventLoop* ev, 
               const voyager::SockAddr& addr,
               const std::string& name,
               const std::vector<std::string>& vec)
      : num_(0),
        client_(ev, addr, name),
        vec_(vec) {
    client_.SetConnectionCallback(
        std::bind(&SudokuClient::ConnectCallback,this,  _1));
    client_.SetMessageCallback(
        std::bind(&SudokuClient::MessageCallback, this, _1, _2));
  }

  SudokuClient(const std::string& name, 
               voyager::EventLoop* ev, 
               const voyager::SockAddr& addr, 
               std::vector<std::string>&& vec)
      : num_(0),
        client_(ev, addr, name),
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
  void ConnectCallback(const voyager::TcpConnectionPtr& ptr) {
    ptr->SetDisConnectionCallback(
        std::bind(&SudokuClient::DisConnectCallback, this, _1));
    VOYAGER_LOG(INFO) << "Start solve sudoku...";
    start_ = voyager::Timestamp::Now();
    for (size_t i = 0; i < vec_.size(); ++i) {
      ptr->SendMessage(vec_.at(i));
    }
  }

  void MessageCallback(const voyager::TcpConnectionPtr& ptr,
                       voyager::Buffer* buf) {
    size_t size = buf->ReadableSize();
    while (size >= kCells + 2) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        std::string res(buf->Peek(), crlf);
        buf->RetrieveUntil(crlf + 2);
        size = buf->ReadableSize();
        VOYAGER_LOG(WARN) << "The result is: \n" << res;
        ++num_;
        if (num_ == static_cast<int64_t>(vec_.size())) {
          stop_ = voyager::Timestamp::Now();
          VOYAGER_LOG(WARN) << "\nStart time is: " << start_.FormatTimestamp()
                            << "\nFinish time is: " << stop_.FormatTimestamp()
                            << "\nTake MicroSeconds: " 
                            << stop_.MicroSecondsSinceEpoch() 
                                   - start_.MicroSecondsSinceEpoch();
          client_.DisConnect();
        }
      } else {
        break;
      }
    }
  }

  void DisConnectCallback(const voyager::TcpConnectionPtr& ptr) {
    ptr->GetLoop()->QueueInLoop(std::bind(&voyager::EventLoop::Exit, ptr->GetLoop()));
  }

  static const int kCells = 81;
  int64_t num_;
  voyager::TcpClient client_;
  std::vector<std::string> vec_;
  voyager::Timestamp start_;
  voyager::Timestamp stop_;

  // No copy allow
  SudokuClient(const SudokuClient&);
  void operator=(const SudokuClient&);
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
  std::vector<std::string> vec;
  for (int i = 0; i < dotimes; ++i) {
    std::string s = voyager::StringPrintf("%d:%s", i+1, message.c_str());
    vec.push_back(std::move(s));
  }
  voyager::EventLoop ev;
  voyager::SockAddr servaddr(argv[1], 5666);
  sudoku::SudokuClient client(&ev, servaddr, "SudokuClinet", std::move(vec));
  client.Connect();
  ev.Loop();
  return 0;
}
