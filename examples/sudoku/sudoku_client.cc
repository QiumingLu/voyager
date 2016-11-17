#include "voyager/core/tcp_client.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/core/callback.h"
#include "voyager/core/buffer.h"
#include "voyager/util/timeops.h"
#include "voyager/util/logging.h"
#include "voyager/util/stringprintf.h"

namespace sudoku {

class SudokuClient {
 public:
  SudokuClient(voyager::EventLoop* ev,
               const voyager::SockAddr& addr,
               const std::string& name,
               const std::vector<std::string>& vec)
      : num_(0),
        start_(0),
        stop_(0),
        client_(ev, addr, name),
        vec_(vec) {
    client_.SetConnectionCallback(
        std::bind(&SudokuClient::ConnectCallback, this, std::placeholders::_1));
    client_.SetMessageCallback(
        std::bind(&SudokuClient::MessageCallback, this,
                  std::placeholders::_1,
                  std::placeholders::_2));
    client_.SetCloseCallback(
        std::bind(&SudokuClient::CloseCallback, this,
                  std::placeholders::_1));
  }

  SudokuClient(voyager::EventLoop* ev,
               const std::string& name,
               const voyager::SockAddr& addr,
               std::vector<std::string>&& vec)
      : num_(0),
        start_(0),
        stop_(0),
        client_(ev, addr, name),
        vec_(std::move(vec)) {
    client_.SetConnectionCallback(std::bind(&SudokuClient::ConnectCallback,
                                            this,
                                            std::placeholders:: _1));
    client_.SetMessageCallback(std::bind(&SudokuClient::MessageCallback,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2));
    client_.SetCloseCallback(std::bind(&SudokuClient::CloseCallback,
                                       this,
                                       std::placeholders::_1));
  }

  void Connect() {
    client_.Connect();
  }

 private:
  void ConnectCallback(const voyager::TcpConnectionPtr& ptr) {
    VOYAGER_LOG(INFO) << "Start solve sudoku...";
    start_ = voyager::timeops::NowMicros();
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
        if (num_ == vec_.size()) {
          stop_ = voyager::timeops::NowMicros();
          VOYAGER_LOG(WARN) << "\nStart time is: "
                            << voyager::timeops::FormatTimestamp(start_)
                            << "\nFinish time is: "
                            << voyager::timeops::FormatTimestamp(stop_)
                            << "\nTake MicroSeconds: " << stop_ - start_;
          client_.Close();
        }
      } else {
        break;
      }
    }
  }

  void CloseCallback(const voyager::TcpConnectionPtr& ptr) {
    ptr->OwnerEventLoop()->Exit();
  }

  static const int kCells = 81;
  size_t num_;
  uint64_t start_;
  uint64_t stop_;
  voyager::TcpClient client_;
  std::vector<std::string> vec_;

  // No copying allowed
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
