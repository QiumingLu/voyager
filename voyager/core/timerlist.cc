// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/timerlist.h"
#include "voyager/core/eventloop.h"
#include "voyager/util/logging.h"
#include "voyager/util/stl_util.h"
#include "voyager/util/timeops.h"


namespace voyager {

TimerList::TimerList(EventLoop* ev)
    : eventloop_(CHECK_NOTNULL(ev)) {
}

TimerList::~TimerList() {
  for (auto& t : timers_) {
    delete t;
  }
}

Timer* TimerList::Insert(uint64_t micros_value,
                         uint64_t micros_interval,
                         const TimerProcCallback& cb) {
  Timer* timer(new Timer(micros_value, micros_interval, cb));
  eventloop_->RunInLoop([this, timer]() {
    InsertInLoop(timer);
  });
  return timer;
}

Timer* TimerList::Insert(uint64_t micros_value,
                         uint64_t micros_interval,
                         TimerProcCallback&& cb) {
  Timer* timer(new Timer(micros_value, micros_interval, std::move(cb)));
  eventloop_->RunInLoop([this, timer]() {
    InsertInLoop(timer);
  });
  return timer;
}

void TimerList::Erase(Timer* timer) {
  eventloop_->RunInLoop([this, timer]() {
    EraseInLoop(timer);
  });
}

void TimerList::InsertInLoop(Timer* timer) {
  eventloop_->AssertInMyLoop();
  timers_.insert(timer);
}

void TimerList::EraseInLoop(Timer* timer) {
  eventloop_->AssertInMyLoop();
  std::set<Timer*>::iterator it = timers_.find(timer);
  if (it != timers_.end()) {
    delete *it;
    timers_.erase(it);
  }
}

uint64_t TimerList::TimeoutMicros() const {
  eventloop_->AssertInMyLoop();
  if (timers_.empty()) {
    return -1;
  }
  std::set<Timer*>::iterator it = timers_.begin();
  if ((*it)->micros_value < timeops::NowMicros()) {
    return 0;
  } else {
    return ((*it)->micros_value - timeops::NowMicros());
  }
}

void TimerList::RunTimerProcs() {
  eventloop_->AssertInMyLoop();
  if (timers_.empty()) {
    return;
  }

  uint64_t micros_now = timeops::NowMicros();
  std::set<Timer*>::iterator it;
  while (true) {
    it = timers_.begin();
    if (it != timers_.end() && (*it)->micros_value <= micros_now) {
      Timer* timer = *it;
      timers_.erase(it);
      timer->timerproc_cb();
      if (timer->repeat) {
        timer->micros_value += timer->micros_interval;
        timers_.insert(timer);
      } else {
        delete timer;
      }
    } else {
      break;
    }
  }
}

}  // namespace voyager
