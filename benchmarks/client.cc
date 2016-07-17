#include "voyager/core/buffer.h"
#include "voyager/core/callback.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/eventloop_threadpool.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_client.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/port/atomic_sequence_num.h"
#include "voyager/util/logging.h"
#include "voyager/util/stl_util.h"
#include "voyager/util/stringprintf.h"

#include <list>
#include <iostream>
#include <fstream>

using namespace std::placeholders;

class Client;

class Session {
 public:
  Session(voyager::EventLoop* ev, 
          const voyager::SockAddr& addr,
          const std::string& name,
          Client* owner)
      : client_(ev, addr, name),
        owner_(owner),
        bytes_read_(0),
        bytes_written_(0) {
    client_.SetConnectionCallback(
        std::bind(&Session::ConnectCallback, this, _1));
    client_.SetMessageCallback(
        std::bind(&Session::MessageCallback, this, _1, _2));
  }

  void Connect() {
    client_.Connect();
  }

  void DisConnect() {
    client_.DisConnect();
  }

  size_t BytesRead() const {
    return bytes_read_;
  }

  size_t BytesWritten() const {
    return bytes_written_;
  }

 private:
  void ConnectCallback(const voyager::TcpConnectionPtr& ptr);
  void DisConnectCallback(const voyager::TcpConnectionPtr& ptr);

  void MessageCallback(const voyager::TcpConnectionPtr& ptr,
                       voyager::Buffer* buf) {
    size_t size = buf->ReadableSize();
    bytes_read_ += size;
    bytes_written_ += size;
    voyager::Slice s(buf->Peek(), size);
    ptr->SendMessage(s);
    buf->Retrieve(size);
  }

  voyager::TcpClient client_;
  Client* owner_;
  size_t bytes_read_;
  size_t bytes_written_;

  // No copy allow
  Session(const Session&);
  void operator=(const Session&);
};

class Client {
 public:
  Client(voyager::EventLoop* ev, 
         const voyager::SockAddr& addr,
         size_t block_size,
         size_t session_count, 
         int timeout,
         int thread_count)
      : base_ev_(ev),
        thread_count_(thread_count),
        ev_pool_(base_ev_, "client", thread_count - 1),
        session_count_(session_count),
        block_size_(block_size),
        timeout_(timeout),
        total_bytes_written(0),
        total_bytes_read(0) {

    ev->RunAfter(timeout, std::bind(&Client::HandleTimeout, this));

    for (size_t i = 0; i < block_size_; ++i) {
      message_.push_back(static_cast<char>(i % 128));
    }

    ev_pool_.Start();
    for (size_t i = 0; i < session_count; ++i) {
      std::string name = voyager::StringPrintf("session %d", i + 1);
      Session* new_session = new Session(ev_pool_.GetNext(), addr, name, this);
      new_session->Connect();
      sessions_.push_back(new_session);
    }
  }

  ~Client() {
    voyager::STLDeleteElements(&sessions_);
  }

  const std::string& Message() const { return message_; }

  void Print(const voyager::TcpConnectionPtr& ptr) {
  
    if (static_cast<size_t>(seq_.GetNext() + 1) == session_count_) {
      for (std::list<Session*>::iterator it = sessions_.begin();
           it != sessions_.end(); ++it) {
        total_bytes_written += (*it)->BytesWritten();
        total_bytes_read += (*it)->BytesRead();
      }

      using namespace voyager;
      VOYAGER_LOG(WARN) << total_bytes_written << " total bytes written";
      VOYAGER_LOG(WARN) << total_bytes_read << " total bytes read";
      VOYAGER_LOG(WARN) << static_cast<double>(total_bytes_read) / (timeout_ * 1024 * 1024)
                        << " MiB/s throughtput";

      std::fstream file;
      file.open("test.txt", std::ofstream::out | std::ofstream::app);
      file << "Bufsize: " << block_size_ << " Threads: " << thread_count_
           << " Sessions: " << sessions_.size() << "\n";
      file << total_bytes_written << " total bytes written\n";
      file << total_bytes_read << " total bytes read\n";
      file << static_cast<double>(total_bytes_read) / (timeout_ * 1024 * 1024) 
           << " MiB/s throughtput\n\n\n";
      file.close();

      ptr->GetLoop()->QueueInLoop(std::bind(&Client::Exit, this));
    }
  }

  void HandleTimeout() {
    std::for_each(sessions_.begin(), sessions_.end(), 
        std::mem_fn(&Session::DisConnect));
  }

 private:
  void Exit() {
    base_ev_->QueueInLoop(std::bind(&voyager::EventLoop::Exit, base_ev_));
  }

  voyager::EventLoop* base_ev_;
  int thread_count_;
  voyager::EventLoopThreadPool ev_pool_;
  size_t session_count_;
  size_t block_size_;
  int timeout_;
  size_t total_bytes_written;
  size_t total_bytes_read;
  std::list<Session*> sessions_;
  std::string message_;
  voyager::port::SequenceNumber seq_;

  // No copy allow
  Client(const Client&);
  void operator=(const Client&);
};

void Session::ConnectCallback(const voyager::TcpConnectionPtr& ptr) {
  ptr->SetCloseCallback(
      std::bind(&Session::DisConnectCallback, this, _1));
  ptr->SetTcpNoDelay(true);
  ptr->SendMessage(owner_->Message());
}

void Session::DisConnectCallback(const voyager::TcpConnectionPtr& ptr) {
  owner_->Print(ptr);
}

int main(int argc, char* argv[]) {
  if (argc != 7) {
    std::cerr << "Usage: client <host> <port> <threads> < blocksize> ";
    std::cerr << "<sessions> <time>\n";
    return 1;
  }
  const char* host = argv[1];
  uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
  int thread_count = atoi(argv[3]);
  size_t block_size = atoi(argv[4]);
  size_t session_count = atoi(argv[5]);
  int timeout = atoi(argv[6]);
  voyager::EventLoop base_ev;
  voyager::SockAddr sockaddr(host, port);
  Client client(&base_ev, sockaddr, block_size,
      session_count, timeout, thread_count);
  base_ev.Loop();
  return 0;
}
