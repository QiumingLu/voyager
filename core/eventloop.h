#ifndef MIRANTS_CORE_EVENTLOOP_H_
#define MIRANTS_CORE_EVENTLOOP_H_

#include <functional>
#include <vector>

#include "util/scoped_ptr.h"
#include "port/currentthread.h"
#include "port/mutex.h"

namespace mirants {

class Dispatch;
class EventPoller;

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

  void RemoveDispatch(Dispatch* dispatch);
  void UpdateDispatch(Dispatch* dispatch);

  void AssertThreadSafe() {
    if (!IsInCreatedThread()) {
      AbortForNotInCreatedThread();
    }
  }

  bool IsInCreatedThread() const { return tid_ == port::CurrentThread::Tid(); }

 private:
  void RunFuncQueue();
  void AbortForNotInCreatedThread();
  
  const pid_t tid_;
  scoped_ptr<EventPoller> poller_;

  port::Mutex mu_;
  std::vector<Func> funcqueue_;

  // No copying allow
  EventLoop(const EventLoop&);
  void operator=(const EventLoop&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENTLOOP_H_
