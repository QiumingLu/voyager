#include "mirants/core/buffer.h"
#include "mirants/core/callback.h"
#include "mirants/core/eventloop.h"
#include "mirants/core/eventloop_threadpool.h"
#include "mirants/core/sockaddr.h"
#include "mirants/core/tcp_client.h"
#include "mirants/core/tcp_connection.h"
#include "mirants/util/logging.h"
#include "mirants/util/stl_util.h"
#include "mirants/util/stringprintf.h"

#include <list>
#include <iostream>
#include <fstream>

using namespace std::placeholders;

class Client;

class Session {
 public:
  Session(mirants::EventLoop* ev, 
          const mirants::SockAddr& addr,
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
  void ConnectCallback(const mirants::TcpConnectionPtr& ptr);
  void DisConnectCallback(const mirants::TcpConnectionPtr& ptr);

  void MessageCallback(const mirants::TcpConnectionPtr& ptr,
                       mirants::Buffer* buf) {
    size_t size = buf->ReadableSize();
    bytes_read_ += size;
    bytes_written_ += size;
    mirants::Slice s(buf->Peek(), size);
    ptr->SendMessage(s);
    buf->Retrieve(size);
  }

  mirants::TcpClient client_;
  Client* owner_;
  size_t bytes_read_;
  size_t bytes_written_;

  // No copy allow
  Session(const Session&);
  void operator=(const Session&);
};

class Client {
 public:
  Client(mirants::EventLoop* ev, 
         const mirants::SockAddr& addr,
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

    ev->RunAfter(timeout, std::bind(&Client::HanleTimeout, this));

    for (size_t i = 0; i < block_size_; ++i) {
      message_.push_back(static_cast<char>(i % 128));
    }

    ev_pool_.Start();
    for (size_t i = 0; i < session_count; ++i) {
      std::string name = mirants::StringPrintf("session %d", i + 1);
      Session* new_session = new Session(ev_pool_.GetNext(), addr, name, this);
      new_session->Connect();
      sessions_.push_back(new_session);
    }
  }

  ~Client() {
    mirants::STLDeleteElements(&sessions_);
  }

  const std::string& Message() const { return message_; }

  void Print(const mirants::TcpConnectionPtr& ptr) {
  
    if (static_cast<size_t>(seq_.GetNext() + 1) == session_count_) {
      for (std::list<Session*>::iterator it = sessions_.begin();
           it != sessions_.end(); ++it) {
        total_bytes_written += (*it)->BytesWritten();
        total_bytes_read += (*it)->BytesRead();
      }

      using namespace mirants;
      MIRANTS_LOG(INFO) << total_bytes_written << " total bytes written";
      MIRANTS_LOG(INFO) << total_bytes_read << " total bytes read";
      MIRANTS_LOG(INFO) << static_cast<double>(total_bytes_read) / (timeout_ * 1024 * 1024)
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

  void HanleTimeout() {
    std::for_each(sessions_.begin(), sessions_.end(), 
        std::mem_fn(&Session::DisConnect));
  }

 private:
  void Exit() {
    base_ev_->QueueInLoop(std::bind(&mirants::EventLoop::Exit, base_ev_));
  }

  mirants::EventLoop* base_ev_;
  int thread_count_;
  mirants::EventLoopThreadPool ev_pool_;
  size_t session_count_;
  size_t block_size_;
  int timeout_;
  size_t total_bytes_written;
  size_t total_bytes_read;
  std::list<Session*> sessions_;
  std::string message_;
  mirants::port::SequenceNumber seq_;

  // No copy allow
  Client(const Client&);
  void operator=(const Client&);
};

void Session::ConnectCallback(const mirants::TcpConnectionPtr& ptr) {
  ptr->SetDisConnectionCallback(
      std::bind(&Session::DisConnectCallback, this, _1));
  ptr->SetTcpNoDelay(true);
  ptr->SendMessage(owner_->Message());
}

void Session::DisConnectCallback(const mirants::TcpConnectionPtr& ptr) {
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
  mirants::EventLoop base_ev;
  mirants::SockAddr sockaddr(host, port);
  Client client(&base_ev, sockaddr, block_size,
      session_count, timeout, thread_count);
  base_ev.Loop();
  sockaddr.FreeAddrinfo();
  
  return 0;
}
