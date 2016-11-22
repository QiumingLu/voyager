#include "voyager/paxos/runloop.h"

#include <functional>

#include "voyager/paxos/instance.h"
#include "voyager/port/mutexlock.h"

namespace voyager {
namespace paxos {

RunLoop::RunLoop(Instance* instance, const std::string& name)
    : exit_(false),
      instance_(instance),
      thread_(std::bind(&RunLoop::ThreadFunc, this), name),
      mutex_(),
      cond_(&mutex_),
      value_() {
}

RunLoop::~RunLoop() {
  if (exit_ != true) {
    exit_ = true;
    thread_.Join();
  }
}

void RunLoop::Loop() {
  assert(!thread_.Started());
  thread_.Start();
}

void RunLoop::Exit() {
  exit_ = true;
}

void RunLoop::NewValue(const Slice& value) {
  port::MutexLock lock(&mutex_);
  value_ = value;
  cond_.Signal();
}

void RunLoop::NewMessage(const Slice& s) {
  port::MutexLock lock(&mutex_);
  Content* content = new Content();
  bool ret = content->ParseFromArray(s.data(), static_cast<int>(s.size()));
  if (ret) {
    contents_.push_back(content);
    cond_.Signal();
  } else {
  }
}

void RunLoop::ThreadFunc() {
  exit_ = false;
  while(!exit_) {
    Content* content = nullptr;
    {
      port::MutexLock lock(&mutex_);
      while (contents_.empty() && value_.empty() ) {
        cond_.Wait();
      }
      if (!contents_.empty()) {
        content = contents_.front();
        contents_.pop_front();
      }
    }

    if (content != nullptr) {
      instance_->HandleMessage(*content);
      delete content;
    }

    if (!value_.empty()) {
      instance_->HandleNewValue(value_);
      value_.clear();
    }
  }
}

}  // namespace paxos
}  // namespace voyager
