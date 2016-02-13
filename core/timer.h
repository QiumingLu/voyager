#ifndef MIRANTS_CORE_TIMER_H_
#define MIRANTS_CORE_TIMER_H_

#include <set>
#include <vector>

#include "core/callback.h"
#include "core/dispatch.h"
#include "port/atomic_sequence_num.h"
#include "util/timestamp.h"

namespace mirants {

struct Timer {
  TimeProcCallback timeproc;
  Timestamp time;
  double interval;
  int64_t timer_id;

  Timer(const TimeProcCallback& func, Timestamp t, double inter, int64_t id)
      : timeproc(func), time(t), interval(inter), timer_id(id) {
  }

  Timer(TimeProcCallback&& func, Timestamp t, double inter, int64_t id)
      : timeproc(std::move(func)), 
        time(t), 
        interval(inter), 
        timer_id(id) {
  }
};

class EventLoop;
class TimerEvent {
 public:
  TimerEvent(EventLoop* ev);
  ~TimerEvent();

  int64_t AddTimer(const TimeProcCallback& func, Timestamp t, double interval);
  int64_t AddTimer(TimeProcCallback&& func, Timestamp t, double interval);
  void DeleteTimer(int64_t id);

 private:
  typedef std::pair<Timestamp, Timer*> Entry;

  void AddTimerInLoop(Timer* timer);
  void DeleteTimerInLoop(int64_t id);
  void HandleRead();
  std::vector<Timer*> GetExpired(Timestamp now);
  void Next(const std::vector<Timer*>& expired, Timestamp now);
  bool Add(Timer* timer);
  
  EventLoop* eventloop_;
  const int timerfd_;
  Dispatch dispatch_;
  std::set<Entry> timers_;
  std::set<Timer*> cancel_timers_;
  bool calling_;

  static port::SequenceNumber seq_;
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TIMER_H_
