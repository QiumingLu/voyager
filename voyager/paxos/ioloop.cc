#include "voyager/paxos/ioloop.h"

namespace voyager {
namespace paxos {

IOLoop::IOLoop(Instance* instance, const std::string& name)
    : instance_(instance),
      thread_(std::bind(&IOLoop::ThreadFunc, this, name)) {
}

IOLoop::~IOLoop() {
  if (exit_ != true) {
    exit_ = true;
    thread_.Join();
  }
}

void IOLoop::Loop() {
  assert(!thread_.Started());
  thread_.Start();
}

void IOLoop::Exit() {
  exit_ = true;
}

void IOLoop::NewMessage(const char* s, size_t n) {
  port::MutexLock lock(&mutex_);
  messages_.push_back(new std::string(s, n));
}

void IOLoop::ThreadFunc() {
  exit_ = false;
  while(!exit_) {
    std::string* s = nullptr;
    {
      MutexLock lock(&mutex_);
      if (!messages_.empty()) {
        s = messages_.front();
        messages_.pop_front();
      }
    }
    if (s != nullptr && !s.empty()) {
      instance_->HandleMessage(*s);
    }
    delete s;
  }
}

}  // namespace paxos
}  // namespace voyager
