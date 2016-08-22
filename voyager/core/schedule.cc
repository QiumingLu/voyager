#include "voyager/core/schedule.h"

#include <utility>

#include "voyager/core/eventloop.h"
#include "voyager/core/online_connections.h"
#include "voyager/util/logging.h"

namespace voyager {

Schedule::Schedule(EventLoop* ev, int size)
    : baseloop_(CHECK_NOTNULL(ev)),
      size_(size),
      next_(0),
      started_(false) {
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
    std::unique_ptr<BGEventLoop> loop(new BGEventLoop());
    ptrs_.push_back(loop->Loop());
    loops_.push_back(std::move(loop));
  }
}

EventLoop* Schedule::AssignLoop() {
  baseloop_->AssertInMyLoop();
  assert(started_);
  if (ptrs_.empty()) {
    return baseloop_;
  }

  EventLoop* loop = NextLoop();

  int run = 0;
  OnlineConnections& instance
      = port::Singleton<OnlineConnections>::Instance();
  size_t all = instance.AllOnlineUsersNum();
  while (all > 0 && run < 3) {
    if (instance.OnlineUserNum(loop) > (percent_ * static_cast<double>(all))) {
      loop = NextLoop();
    } else {
      break;
    }
    ++run;
  }

  return loop;
}

EventLoop* Schedule::NextLoop() {
  if (next_ >= ptrs_.size()) {
    next_ = 0;
  }
  return ptrs_[next_++];
}

}  // namespace voyager
