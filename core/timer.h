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
  bool repeat;

  Timer(const TimeProcCallback& func, Timestamp t, double inter, int64_t id)
      : timeproc(func), time(t), interval(inter), timer_id(id), repeat(false) {
    if (interval > 0.0) {
      repeat = true;
    }
  }

  Timer(TimeProcCallback&& func, Timestamp t, double inter, int64_t id)
      : timeproc(std::move(func)), 
        time(t), 
        interval(inter), 
        timer_id(id),
        repeat(false) {
    if (interval > 0.0) {
      repeat = true;
    }
  }
};

class EventLoop;
class TimerEvent {
 public:
  TimerEvent(EventLoop* ev);
  ~TimerEvent();

  Timer* AddTimer(const TimeProcCallback& func, Timestamp t, double interval);
  Timer* AddTimer(TimeProcCallback&& func, Timestamp t, double interval);
  void DeleteTimer(Timer* timer);

 private:
  typedef std::pair<Timestamp, Timer*> Entry;

  void AddTimerInLoop(Timer* timer);
  void DeleteTimerInLoop(Timer* timer);
  void HandleRead();
  std::vector<Timer*> GetExpired(Timestamp now);
  void Next(const std::vector<Timer*>& expired, Timestamp now);
  bool Add(Timer* timer);
  
  EventLoop* eventloop_;
  const int timerfd_;
  Dispatch dispatch_;
  bool calling_;
  std::set<Entry> timers_;
  std::set<Timer*> delete_timers_;
 
  static port::SequenceNumber seq_;
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TIMER_H_
