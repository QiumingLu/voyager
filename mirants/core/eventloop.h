#ifndef MIRANTS_CORE_EVENTLOOP_H_
#define MIRANTS_CORE_EVENTLOOP_H_

#include <functional>
#include <vector>

#include "mirants/core/callback.h"
#include "mirants/core/timer.h"
#include "mirants/util/scoped_ptr.h"
#include "mirants/port/currentthread.h"
#include "mirants/port/mutex.h"

namespace mirants {

class Dispatch;
class EventPoller;
class Timestamp;

class EventLoop {
 public:
  typedef std::function<void()> Func;
  EventLoop();
  ~EventLoop();
  void Loop();
  void RunInLoop(const Func& func);
  void RunInLoop(Func&& func);
  void QueueInLoop(const Func& func);
  void QueueInLoop(Func&& func);

  Timer* RunAt(const Timestamp& t, const TimeProcCallback& timeproc);
  Timer* RunAfter(double delay, const TimeProcCallback& timeproc);
  Timer* RunEvery(double interval, const TimeProcCallback& timeproc);
  
  Timer* RunAt(const Timestamp& t, TimeProcCallback&& timeproc);
  Timer* RunAfter(double delay, TimeProcCallback&& timeproc);
  Timer* RunEvery(double interval, TimeProcCallback&& timeproc);

  void DeleteTimer(Timer* t);

  void RemoveDispatch(Dispatch* dispatch);
  void UpdateDispatch(Dispatch* dispatch);

  void AssertThreadSafe() {
    if (!IsInCreatedThread()) {
      AbortForNotInCreatedThread();
    }
  }

  bool IsInCreatedThread() const { return tid_ == port::CurrentThread::Tid(); }

  void Exit() { exit_ = true; }

 private:
  void RunFuncQueue();
  void AbortForNotInCreatedThread();
  
  const pid_t tid_;
  scoped_ptr<EventPoller> poller_;

  scoped_ptr<TimerEvent> timer_ev_;
  bool exit_;

  port::Mutex mu_;
  std::vector<Func> funcqueue_;

  // No copying allow
  EventLoop(const EventLoop&);
  void operator=(const EventLoop&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENTLOOP_H_
