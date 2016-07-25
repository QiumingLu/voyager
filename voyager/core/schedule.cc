#include "voyager/core/schedule.h"
#include "voyager/core/eventloop.h"

namespace voyager {

Schedule::Schedule(EventLoop* baseloop, int size)
    : baseloop_(baseloop),
      size_(size),
      next_(0), 
      started_(false),
      loops_(new scoped_ptr<BGEventLoop>[size]) {
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
  if (next_ >= ptrs_.size()) {
    next_ = 0;
  }
  return ptrs_[next_++];
}

}  // namespace voyager
