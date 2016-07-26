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
    percent_ = 2.0;
  } else if (size_ == 2) {
    percent_ = 5.0 / 8.0;
  } else if (size_ == 3) {
    percent_ =  4.0 / 9.0;
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

  EventLoop* loop = Next();

  int run = 0;
  OnlineConnections& instance 
      = port::Singleton<OnlineConnections>::Instance();
  size_t all = instance.AllOnlineUsersNum();
  while (all > 0 && run < 3) {
    if (instance.OnlineUserNum(loop) > (percent_ * all)) {
      loop = Next();
    } else {
      break;
    }
    ++run;
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
