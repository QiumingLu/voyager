#include "examples/sudoku/sudoku_solver.h"
#include "voyager/core/buffer.h"
#include "voyager/core/callback.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_server.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/util/logging.h"
#include "voyager/util/string_util.h"

namespace sudoku {

class SudukuServer {
 public:
  SudukuServer(voyager::EventLoop* ev, const voyager::SockAddr& addr)
      : server_(ev, addr, "SudukuServer", 5) {
    
    using namespace std::placeholders;
    server_.SetConnectionCallback(
        std::bind(&SudukuServer::ConnectCallback, this, _1));
    server_.SetMessageCallback(
        std::bind(&SudukuServer::MessageCallback, this, _1, _2));
  }

  void Start() {
    server_.Start();
  }

 private:
  void ConnectCallback(const voyager::TcpConnectionPtr& ptr) {
  }

  void MessageCallback(const voyager::TcpConnectionPtr& ptr, 
                       voyager::Buffer* buf) {
    size_t size = buf->ReadableSize();
    while (size >= kCells + 2) {
      const char* crlf = buf->FindCRLF();
      if (crlf) {
        std::string s(buf->Peek(), crlf);
        buf->RetrieveUntil(crlf + 2);
        size = buf->ReadableSize();

        if (!Slove(ptr, std::move(s))) {
          voyager::Slice bad("Bad Request!\r\n");
          ptr->SendMessage(bad);
          ptr->ShutDown();
          break;
        }
      } else {
        break;
      }
    }
  }

  bool Slove(const voyager::TcpConnectionPtr& ptr, std::string&& s) {
    std::string id;
    std::string puzzle;
    std::vector<std::string> v;
    voyager::SplitStringUsing(s, ":", &v);
    if (v.size() > 1) {
      id = v.at(0);
      puzzle = v.at(1);
    } else {
      puzzle = std::move(s);
    }

    if (puzzle.size() != static_cast<size_t>(kCells)) {
      return false;
    }
    SudokuSolver solver(puzzle);
    std::string res = solver.Solve();
    if (id.empty()) {
      ptr->SendMessage(res + "\r\n");
    } else {
      ptr->SendMessage(id + ":" + res + "\r\n");
    }
    return true;
  }

  const static int kCells = 81;

  voyager::TcpServer server_;
};

}  // namespace sudoku

int main(int argc, char** argv) {
  voyager::EventLoop ev;
  voyager::SockAddr addr(5666);
  sudoku::SudukuServer server(&ev, addr);
  server.Start();
  ev.Loop();
  return 0;
}
