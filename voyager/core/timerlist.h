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

class Timer;

typedef std::pair<uint64_t, Timer*> TimerId;

class TimerList {
 public:
  explicit TimerList(EventLoop* ev);
  ~TimerList();

  TimerId Insert(uint64_t micros_value, uint64_t micros_interval,
                 const TimerProcCallback& cb);
  TimerId Insert(uint64_t micros_value, uint64_t micros_interval,
                 TimerProcCallback&& cb);
  void Erase(TimerId timer);

  uint64_t TimeoutMicros() const;
  void RunTimerProcs();

 private:
  void InsertInLoop(TimerId timer);
  void EraseInLoop(TimerId timer);

  EventLoop* eventloop_;

  std::set<Timer*> timer_ptrs_;
  std::set<TimerId> timers_;

  // No copying allowed
  TimerList(const TimerList&);
  void operator=(const TimerList&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TIMERLIST_H_
