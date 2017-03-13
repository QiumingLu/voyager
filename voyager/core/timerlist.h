// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_TIMERLIST_H_
#define VOYAGER_CORE_TIMERLIST_H_

#include <set>
#include <vector>
#include <utility>

#include "voyager/core/callback.h"

namespace voyager {

class EventLoop;

struct TimerCompare;
class TimerList;

class Timer {
 private:
  friend struct TimerCompare;
  friend class TimerList;

  Timer(uint64_t value, uint64_t interval, const TimerProcCallback& cb)
      : micros_value(value),
        micros_interval(interval),
        timerproc_cb(cb),
        repeat(false) {
    if (micros_interval > 0) {
      repeat = true;
    }
  }

  Timer(uint64_t value, uint64_t interval, TimerProcCallback&& cb)
      : micros_value(value),
        micros_interval(interval),
        timerproc_cb(std::move(cb)),
        repeat(false) {
    if (micros_interval > 0) {
      repeat = true;
    }
  }

  ~Timer() {
  }

  uint64_t micros_value;
  uint64_t micros_interval;
  TimerProcCallback timerproc_cb;
  bool repeat;
};

struct TimerCompare {
  bool operator() (const Timer* lhs, const Timer* rhs) {
    if (lhs->micros_value == rhs->micros_value) {
      return lhs < rhs;
    }
    return lhs->micros_value < rhs->micros_value;
  }
};

class TimerList {
 public:
  explicit TimerList(EventLoop* ev);
  ~TimerList();

  Timer* Insert(uint64_t micros_value, uint64_t micros_interval,
                const TimerProcCallback& cb);
  Timer* Insert(uint64_t micros_value, uint64_t micros_interval,
                TimerProcCallback&& cb);
  void Erase(Timer* timer);

  uint64_t TimeoutMicros() const;
  void RunTimerProcs();

 private:
  void InsertInLoop(Timer* timer);
  void EraseInLoop(Timer* timer);

  EventLoop* eventloop_;
  std::set<Timer*, TimerCompare> timers_;

  // No copying allowed
  TimerList(const TimerList&);
  void operator=(const TimerList&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TIMERLIST_H_
