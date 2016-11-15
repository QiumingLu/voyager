#include "voyager/paxos/ioloop.h"

#include <functional>

#include "voyager/paxos/instance.h"
#include "voyager/port/mutexlock.h"

namespace voyager {
namespace paxos {

IOLoop::IOLoop(Instance* instance, const std::string& name)
    : exit_(false),
      instance_(instance),
      thread_(std::bind(&IOLoop::ThreadFunc, this), name),
      mutex_(),
      cond_(&mutex_),
      value_() {
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

void IOLoop::NewValue(const Slice& value) {
  port::MutexLock lock(&mutex_);
  value_ = value;
  cond_.Signal();
}

void IOLoop::NewMessage(const Slice& s) {
  port::MutexLock lock(&mutex_);
  messages_.push_back(new std::string(s.data(), s.size()));
  cond_.Signal();
}

void IOLoop::ThreadFunc() {
  exit_ = false;
  while(!exit_) {
    std::string* s = nullptr;
    {
      port::MutexLock lock(&mutex_);
      while (messages_.empty() && value_.empty() ) {
        cond_.Wait();
      }
      if (!messages_.empty()) {
        s = messages_.front();
        messages_.pop_front();
      }
    }

    if (s != nullptr) {
      instance_->HandleMessage(*s);
      delete s;
    }

    if (!value_.empty()) {
      instance_->HandleNewValue(value_);
      value_.clear();
    }
  }
}

}  // namespace paxos
}  // namespace voyager
