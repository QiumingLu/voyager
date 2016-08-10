#ifndef VOYAGER_CORE_TIMERLIST_H_
#define VOYAGER_CORE_TIMERLIST_H_

#include <set>
#include <vector>

#include "voyager/core/callback.h"

namespace voyager {

class EventLoop;

class TimerList {
 public:
  struct Timer;

  TimerList(EventLoop* ev);
  ~TimerList();

  Timer* Insert(const TimerProcCallback& cb, 
                uint64_t micros_value, uint64_t micros_interval);
  Timer* Insert(TimerProcCallback&& cb,
                uint64_t micros_value, uint64_t micros_interval);
  void Erase(Timer* timer);

  uint64_t TimeoutMicros() const;
  void RunTimerProcs();

 private:
  typedef std::pair<uint64_t, Timer*> Entry;

  void InsertInLoop(Timer* timer);
  void EraseInLoop(Timer* timer);

  std::vector<Timer*> ExpiredTimers(uint64_t micros);
  
  EventLoop* eventloop_;
  std::set<Entry> timers_;

  // No copy allow
  TimerList(const TimerList&);
  void operator=(const TimerList&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TIMERLIST_H_
