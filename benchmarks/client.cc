// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <atomic>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

#include "voyager/core/buffer.h"
#include "voyager/core/callback.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/schedule.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_client.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/util/logging.h"
#include "voyager/util/stl_util.h"
#include "voyager/util/stringprintf.h"

using namespace std::placeholders;

namespace voyager {

class Client;

class Session {
 public:
  Session(voyager::EventLoop* ev, const voyager::SockAddr& addr,
          const std::string& name, Client* owner)
      : client_(ev, addr, name),
        owner_(owner),
        bytes_read_(0),
        bytes_written_(0) {
    client_.SetConnectionCallback(
        std::bind(&Session::ConnectCallback, this, _1));
    client_.SetMessageCallback(
        std::bind(&Session::MessageCallback, this, _1, _2));
    client_.SetCloseCallback(std::bind(&Session::CloseCallback, this, _1));
  }

  void Connect() { client_.Connect(); }

  void Close() { client_.Close(); }

  size_t BytesRead() const { return bytes_read_; }

  size_t BytesWritten() const { return bytes_written_; }

 private:
  void ConnectCallback(const TcpConnectionPtr& ptr);
  void CloseCallback(const TcpConnectionPtr& ptr);

  void MessageCallback(const TcpConnectionPtr& ptr, Buffer* buf) {
    size_t size = buf->ReadableSize();
    bytes_read_ += size;
    bytes_written_ += size;
    Slice s(buf->Peek(), size);
    ptr->SendMessage(s);
    buf->Retrieve(size);
  }

  voyager::TcpClient client_;
  Client* owner_;
  size_t bytes_read_;
  size_t bytes_written_;

  // No copying allowed
  Session(const Session&);
  void operator=(const Session&);
};

class Client {
 public:
  Client(EventLoop* ev, const SockAddr& addr, int block_size, int session_count,
         uint64_t timeout, int thread_count)
      : base_ev_(ev),
        thread_count_(thread_count),
        session_count_(session_count),
        block_size_(block_size),
        total_bytes_written(0),
        total_bytes_read(0),
        timeout_(timeout),
        seq_(0),
        schedule_(base_ev_, thread_count - 1) {
    ev->RunAfter(timeout * 1000, [this]() { this->HandleTimeout(); });
    message_.resize(block_size_);
    for (int i = 0; i < block_size_; ++i) {
      message_.push_back(static_cast<char>(i % 128));
    }

    schedule_.Start();
    for (int i = 0; i < session_count; ++i) {
      std::string name = StringPrintf("session %d", i + 1);
      Session* new_session =
          new Session(schedule_.AssignLoop(), addr, name, this);
      new_session->Connect();
      sessions_.push_back(new_session);
    }
  }

  ~Client() { STLDeleteElements(&sessions_); }

  const std::string& Message() const { return message_; }

  void Print(const TcpConnectionPtr& ptr) {
    if (++seq_ < session_count_) return;

    for (std::vector<Session*>::iterator it = sessions_.begin();
         it != sessions_.end(); ++it) {
      total_bytes_written += (*it)->BytesWritten();
      total_bytes_read += (*it)->BytesRead();
    }

    VOYAGER_LOG(WARN) << total_bytes_written << " total bytes written";
    VOYAGER_LOG(WARN) << total_bytes_read << " total bytes read";
    VOYAGER_LOG(WARN) << static_cast<double>(total_bytes_read) /
                             static_cast<double>(timeout_ * 1024 * 1024)
                      << " MiB/s throughtput";

    std::fstream file;
    file.open("test.txt", std::ofstream::out | std::ofstream::app);
    file << "Bufsize: " << block_size_ << " Threads: " << thread_count_
         << " Sessions: " << sessions_.size() << "\n";
    file << total_bytes_written << " total bytes written\n";
    file << total_bytes_read << " total bytes read\n";
    file << static_cast<double>(total_bytes_read) /
                static_cast<double>(timeout_ * 1024 * 1024)
         << " MiB/s throughtput\n\n\n";
    file.close();
    base_ev_->Exit();
  }

  void HandleTimeout() {
    std::for_each(sessions_.begin(), sessions_.end(),
                  std::mem_fn(&Session::Close));
  }

 private:
  EventLoop* base_ev_;
  int thread_count_;
  int session_count_;
  int block_size_;
  size_t total_bytes_written;
  size_t total_bytes_read;
  uint64_t timeout_;
  std::atomic<int> seq_;
  Schedule schedule_;
  std::vector<Session*> sessions_;
  std::string message_;

  // No copying allowed
  Client(const Client&);
  void operator=(const Client&);
};

void Session::ConnectCallback(const TcpConnectionPtr& ptr) {
  ptr->SendMessage(owner_->Message());
}

void Session::CloseCallback(const TcpConnectionPtr& ptr) { owner_->Print(ptr); }

}  // namespace voyager

int main(int argc, char* argv[]) {
  if (argc != 7) {
    std::cerr << "Usage: client <host> <port> <threads> <blocksize> ";
    std::cerr << "<sessions> <time>\n";
    return 1;
  }
  const char* host = argv[1];
  uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
  int thread_count = atoi(argv[3]);
  int block_size = atoi(argv[4]);
  int session_count = atoi(argv[5]);
  int timeout = atoi(argv[6]);
  voyager::EventLoop base_ev;
  voyager::SockAddr sockaddr(host, port);
  voyager::Client client(&base_ev, sockaddr, block_size, session_count, timeout,
                         thread_count);
  base_ev.Loop();
  return 0;
}
