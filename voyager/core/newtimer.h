#ifndef VOYAGER_CORE_NEWTIMER_H_
#define VOYAGER_CORE_NEWTIMER_H_

#include <stdint.h>

#include "voyager/core/callback.h"
#include "voyager/core/dispatch.h"

namespace voyager {

class NewTimer {
 public:
  NewTimer(EventLoop* ev, const TimerProcCallback& cb);
  NewTimer(EventLoop* ev, TimerProcCallback&& cb);
  ~NewTimer();

  EventLoop* OwnerLoop() const { return eventloop_; }
  bool IsRepeat() const { return repeat_; }

  // (1) 当 nanos_interval 不为 0 时，表示为周期定时
  // (2) 当 nanos_value和nanos_interval 都为 0 时，表示不再定时
  void SetTime(uint64_t nanos_value, uint64_t nanos_interval);

 private:
  void SetTimeInLoop(uint64_t nanos_value, uint64_t nanos_interval);
  void HandleRead();

  const int timerfd_;
  bool repeat_;
  bool set_time_;
  EventLoop* eventloop_;
  Dispatch dispatch_;

  TimerProcCallback timerproc_cb_;

  // No copying allow
  NewTimer(const NewTimer&);
  void operator=(const NewTimer&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_NEWTIMER_H_