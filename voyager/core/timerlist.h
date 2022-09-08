// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_TIMERLIST_H_
#define VOYAGER_CORE_TIMERLIST_H_

#include <set>
#include <utility>
#include <vector>

#include "voyager/core/callback.h"
#include "voyager/core/eventloop.h"

namespace voyager {

class TimerList {
 public:
  explicit TimerList(EventLoop* ev);
  ~TimerList();

  TimerId Insert(uint64_t ms_value, uint64_t ms_interval,
                 const TimerProcCallback& cb);
  TimerId Insert(uint64_t ms_value, uint64_t ms_interval,
                 TimerProcCallback&& cb);
  void Erase(TimerId timer);

  uint64_t TimeoutMs() const;
  void RunTimerProcs();

 private:
  void InsertInLoop(TimerId timer);
  void EraseInLoop(TimerId timer);

  uint64_t last_time_out_;

  EventLoop* eventloop_;

  std::set<Timer*> timer_ptrs_;
  std::set<TimerId> timers_;

  // No copying allowed
  TimerList(const TimerList&);
  void operator=(const TimerList&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TIMERLIST_H_
