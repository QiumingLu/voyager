// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/timerlist.h"
#include "voyager/util/logging.h"
#include "voyager/util/timeops.h"

namespace voyager {

class Timer {
 private:
  friend class TimerList;

  Timer(uint64_t value, uint64_t interval, const TimerProcCallback& cb)
      : micros_value(value), micros_interval(interval), timerproc_cb(cb) {}

  Timer(uint64_t value, uint64_t interval, TimerProcCallback&& cb)
      : micros_value(value),
        micros_interval(interval),
        timerproc_cb(std::move(cb)) {}

  ~Timer() {}

  uint64_t micros_value;
  uint64_t micros_interval;
  TimerProcCallback timerproc_cb;
};

TimerList::TimerList(EventLoop* ev)
    : last_time_out_(timeops::NowMicros()), eventloop_(CHECK_NOTNULL(ev)) {}

TimerList::~TimerList() {
  for (auto& t : timer_ptrs_) {
    delete t;
  }
}

TimerId TimerList::Insert(uint64_t micros_value, uint64_t micros_interval,
                          const TimerProcCallback& cb) {
  TimerId timer(micros_value, new Timer(micros_value, micros_interval, cb));
  eventloop_->RunInLoop([this, timer]() { InsertInLoop(timer); });
  return timer;
}

TimerId TimerList::Insert(uint64_t micros_value, uint64_t micros_interval,
                          TimerProcCallback&& cb) {
  TimerId timer(micros_value,
                new Timer(micros_value, micros_interval, std::move(cb)));
  eventloop_->RunInLoop([this, timer]() { InsertInLoop(timer); });
  return timer;
}

void TimerList::Erase(TimerId timer) {
  eventloop_->RunInLoop([this, timer]() { EraseInLoop(timer); });
}

void TimerList::InsertInLoop(TimerId timer) {
  eventloop_->AssertInMyLoop();
  timers_.insert(timer);
  timer_ptrs_.insert(timer.second);
}

void TimerList::EraseInLoop(TimerId timer) {
  eventloop_->AssertInMyLoop();
  std::set<Timer*>::iterator it = timer_ptrs_.find(timer.second);
  if (it != timer_ptrs_.end()) {
    timer.first = timer.second->micros_value;
    timers_.erase(timer);
    delete *it;
    timer_ptrs_.erase(it);
  }
}

uint64_t TimerList::TimeoutMicros() const {
  eventloop_->AssertInMyLoop();
  if (timers_.empty()) {
    return -1;
  }
  std::set<TimerId>::const_iterator it = timers_.begin();
  uint64_t now = timeops::NowMicros();
  if (now < last_time_out_) {
    return 0;
  }
  if (it->first <= now) {
    return 0;
  } else {
    return (it->first - now);
  }
}

void TimerList::RunTimerProcs() {
  eventloop_->AssertInMyLoop();
  if (timers_.empty()) {
    return;
  }

  uint64_t micros_now = timeops::NowMicros();

  if (micros_now < last_time_out_) {
    uint64_t diff = last_time_out_ - micros_now;
    std::set<TimerId> timers;
    for (auto& it : timers_) {
      uint64_t value = it.first > diff ? it.first - diff : 0;
      it.second->micros_value = value;
      timers.insert(TimerId(value, it.second));
    }
    timers_.swap(timers);
  }
  last_time_out_ = micros_now;

  std::set<TimerId>::iterator it;
  while (true) {
    it = timers_.begin();
    if (it != timers_.end() && it->first <= micros_now) {
      Timer* t = it->second;
      timers_.erase(it);
      TimerProcCallback cb = t->timerproc_cb;
      if (t->micros_interval > 0) {
        t->micros_value = micros_now + t->micros_interval;
        TimerId timer(t->micros_value, t);
        timers_.insert(timer);
      } else {
        delete t;
        timer_ptrs_.erase(t);
      }
      cb();
    } else {
      break;
    }
  }
}

}  // namespace voyager
