// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/bg_eventloop.h"

#include <assert.h>

#include "voyager/port/mutexlock.h"
#include "voyager/util/logging.h"

namespace voyager {

BGEventLoop::BGEventLoop(PollType type, const std::string& name)
    : type_(type),
      eventloop_(nullptr),
      mu_(),
      cond_(&mu_),
      thread_(std::bind(&BGEventLoop::ThreadFunc, this), name) {}

BGEventLoop::~BGEventLoop() {
  if (eventloop_ != nullptr) {
    eventloop_->Exit();
    thread_.Join();
  }
}

EventLoop* BGEventLoop::Loop() {
  assert(!thread_.Started());
  thread_.Start();
  {
    port::MutexLock l(&mu_);
    while (eventloop_ == nullptr) {
      cond_.Wait();
    }
  }
  return eventloop_;
}

void BGEventLoop::ThreadFunc() {
  EventLoop ev(type_);
  {
    port::MutexLock l(&mu_);
    eventloop_ = &ev;
    cond_.Signal();
  }
  eventloop_->Loop();
  eventloop_ = nullptr;
}

}  // namespace voyager
