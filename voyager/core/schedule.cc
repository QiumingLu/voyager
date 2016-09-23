#include "voyager/core/schedule.h"

#include <assert.h>
#include <utility>

#include "voyager/core/eventloop.h"
#include "voyager/util/logging.h"

namespace voyager {

Schedule::Schedule(EventLoop* ev, int size)
    : baseloop_(CHECK_NOTNULL(ev)),
      size_(size),
      started_(false) {
}

void Schedule::Start() {
  assert(!started_);
  baseloop_->AssertInMyLoop();
  started_ = true;
  for (size_t i = 0; i < size_; ++i) {
    std::unique_ptr<BGEventLoop> loop(new BGEventLoop());
    loops_.push_back(loop->Loop());
    bg_loops_.push_back(std::move(loop));
  }
  if (size_ == 0) {
    loops_.push_back(baseloop_);
  }
}

EventLoop* Schedule::AssignLoop() {
  baseloop_->AssertInMyLoop();
  assert(started_);
  assert(!loops_.empty());
  EventLoop* loop = loops_[0];
  int min = loop->UserNumber();
  size_t i = 1;
  while (i < loops_.size()) {
    if (loops_[i]->UserNumber() < min) {
      loop = loops_[i];
    }
    ++i;
  }

  assert(loop != nullptr);
  return loop;
}

}  // namespace voyager
