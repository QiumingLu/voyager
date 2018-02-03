// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/schedule.h"

#include <assert.h>
#include <utility>

namespace voyager {

Schedule::Schedule(EventLoop* ev, int size)
    : started_(false), baseloop_(ev), size_(size), next_(0) {}

void Schedule::Start() {
  assert(!started_);
  started_ = true;
  loops_.reserve(size_);
  bg_loops_.reserve(size_);
  for (int i = 0; i < size_; ++i) {
    BGEventLoop* loop = new BGEventLoop(baseloop_->GetPollType());
    loops_.push_back(loop->Loop());
    bg_loops_.push_back(std::unique_ptr<BGEventLoop>(loop));
  }
  if (size_ == 0) {
    loops_.push_back(baseloop_);
  }
}

const std::vector<EventLoop*>* Schedule::AllLoops() const {
  assert(started_);
  return &loops_;
}

EventLoop* Schedule::AssignLoop() {
  baseloop_->AssertInMyLoop();
  if (size_ > 5) {
    return GetNextLoop();
  } else {
    return GetMinLoop();
  }
}

EventLoop* Schedule::GetNextLoop() {
  assert(started_);
  assert(!loops_.empty());
  if (next_ >= static_cast<int>(loops_.size())) {
    next_ = 0;
  }
  return loops_[next_++];
}

EventLoop* Schedule::GetMinLoop() {
  assert(started_);
  assert(!loops_.empty());
  EventLoop* loop = loops_[0];
  int min = loop->ConnectionSize();

  for (size_t i = 1; i < loops_.size(); ++i) {
    int temp = loops_[i]->ConnectionSize();
    if (temp < min) {
      min = temp;
      loop = loops_[i];
    }
  }

  assert(loop != nullptr);
  return loop;
}

}  // namespace voyager
