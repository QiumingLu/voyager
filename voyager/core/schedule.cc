#include "voyager/core/schedule.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/online_connections.h"

namespace voyager {

Schedule::Schedule(EventLoop* baseloop, int size)
    : baseloop_(baseloop),
      size_(size),
      next_(0), 
      started_(false),
      loops_(new scoped_ptr<BGEventLoop>[size]) {
  if (size_ == 1) {
    percent_ = 1.0;
  } else if (size_ == 2) {
    percent_ = 6.0 / 8.0;
  } else if (size_ == 3) {
    percent_ =  4.0 / 8.0;
  } else {
    percent_ = 3.0 / 8.0;
  }
}

void Schedule::Start() {
  assert(!started_);
  baseloop_->AssertInMyLoop();
  started_ = true;
  for (size_t i = 0; i < size_; ++i) {
    BGEventLoop* loop = new BGEventLoop();
    loops_[i].reset(loop);
    ptrs_.push_back(loop->StartLoop());
  }
}

EventLoop* Schedule::AssignLoop() {
  baseloop_->AssertInMyLoop();
  assert(started_);
  if (ptrs_.empty()) {
    return baseloop_;
  }

  int test = 0;
  EventLoop* loop = Next();

  OnlineConnections& instance = port::Singleton<OnlineConnections>::Instance();
  while (instance.AllOnlineUsersNum() > 0 && test < 3) {
    if (instance.OnlineUserNum(loop) > (percent_ * instance.AllOnlineUsersNum())) {
      loop = Next();
    } else {
      break;
    }
    ++test;
  }

  return loop;
}

EventLoop* Schedule::Next() {
  if (next_ >= ptrs_.size()) {
    next_ = 0;
  }
  return ptrs_[next_++];
}

}  // namespace voyager
