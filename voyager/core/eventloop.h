#ifndef VOYAGER_CORE_EVENTLOOP_H_
#define VOYAGER_CORE_EVENTLOOP_H_

#include <functional>
#include <vector>

#include "voyager/core/timerlist.h"
#include "voyager/port/currentthread.h"
#include "voyager/port/mutexlock.h"
#include "voyager/util/scoped_ptr.h"

namespace voyager {

class Dispatch;
class EventPoller;

class EventLoop {
 public:
  typedef std::function<void()> Func;

  EventLoop();
  ~EventLoop();
 
  void Loop();
 
  void RunInLoop(const Func& func);
  void QueueInLoop(const Func& func);

  void RunInLoop(Func&& func);
  void QueueInLoop(Func&& func);

  TimerList::Timer* RunAt(const TimerProcCallback& cb, 
                          uint64_t micros_value);
  TimerList::Timer* RunAfter(const TimerProcCallback& cb, 
                             uint64_t micros_delay);
  TimerList::Timer* RunEvery(const TimerProcCallback& cb,
                             uint64_t micros_interval);
  
  TimerList::Timer* RunAt(TimerProcCallback&& cb, uint64_t micros_value);
  TimerList::Timer* RunAfter(TimerProcCallback&& cb, uint64_t micros_delay);
  TimerList::Timer* RunEvery(TimerProcCallback&& cb, uint64_t micros_interval);

  void RemoveTimer(TimerList::Timer* t);

  void AssertInMyLoop() {
    if (!IsInMyLoop()) {
      Abort();
    }
  }

  bool IsInMyLoop() const { return tid_ == port::CurrentThread::Tid(); }

  void Exit();

  // the eventloop of current thread.
  static EventLoop* RunLoop();
  
  // only internal use
  void RemoveDispatch(Dispatch* dispatch);
  void UpdateDispatch(Dispatch* dispatch);
  bool HasDispatch(Dispatch* dispatch);

 private:
  void RunFuncs();
  void HandleRead();
  void Abort();
  void WakeUp();
 
  bool exit_;
  bool run_;

  const uint64_t tid_;
  scoped_ptr<EventPoller> poller_;
  scoped_ptr<TimerList> timers_;

  #ifdef __linux__
  int wakeup_fd_;
  #else
  int wakeup_fd_[2];
  #endif

  scoped_ptr<Dispatch> wakeup_dispatch_;

  port::Mutex mu_;
  std::vector<Func> funcs_;

  // No copying allow
  EventLoop(const EventLoop&);
  void operator=(const EventLoop&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENTLOOP_H_
