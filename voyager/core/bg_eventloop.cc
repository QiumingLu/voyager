// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/bg_eventloop.h"

#include <assert.h>
#include <functional>

namespace voyager {

BGEventLoop::BGEventLoop(PollType type) : type_(type), eventloop_(nullptr) {}

BGEventLoop::~BGEventLoop() {
  if (eventloop_ != nullptr) {
    eventloop_->Exit();
    thread_->join();
  }
}

EventLoop* BGEventLoop::Loop() {
  assert(!thread_);
  thread_.reset(new std::thread(std::bind(&BGEventLoop::ThreadFunc, this)));
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (eventloop_ == nullptr) {
      cv_.wait(lock);
    }
  }
  return eventloop_;
}

void BGEventLoop::ThreadFunc() {
  EventLoop ev(type_);
  {
    std::unique_lock<std::mutex> lock(mutex_);
    eventloop_ = &ev;
    cv_.notify_one();
  }
  eventloop_->Loop();
  eventloop_ = nullptr;
}

}  // namespace voyager
