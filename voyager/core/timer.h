#ifndef VOYAGER_CORE_TIMER_H_
#define VOYAGER_CORE_TIMER_H_

#include <set>
#include <vector>

#include "voyager/core/callback.h"
#include "voyager/core/dispatch.h"
#include "voyager/util/timestamp.h"

namespace voyager {

struct Timer;
class EventLoop;
class TimerEvent {
 public:
  TimerEvent(EventLoop* ev);
  ~TimerEvent();

  Timer* AddTimer(const TimeProcCallback& func, Timestamp t, double interval);
  Timer* AddTimer(TimeProcCallback&& func, Timestamp t, double interval);
  void DeleteTimer(Timer* timer);

#ifdef __APPLE__
  int GetTimeout() const;
  void HandleTimers();
#endif

 private:
  typedef std::pair<Timestamp, Timer*> Entry;

  void AddTimerInLoop(Timer* timer);
  void DeleteTimerInLoop(Timer* timer);

#ifdef __linux__
  void HandleRead();
#endif

  std::vector<Timer*> GetExpired(Timestamp now);
  void Next(const std::vector<Timer*>& expired, Timestamp now);
  bool Add(Timer* timer);
  
  EventLoop* eventloop_;

#ifdef __linux__
  const int timerfd_;
  Dispatch dispatch_;
#endif

  bool calling_;
  std::set<Entry> timers_;
  std::set<Timer*> delete_timers_;

  // No copy allow
  TimerEvent(const TimerEvent&);
  void operator=(const TimerEvent&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TIMER_H_
