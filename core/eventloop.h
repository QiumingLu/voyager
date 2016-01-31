#ifndef MIRANTS_CORE_EVENTLOOP_H_
#define MIRANTS_CORE_EVENTLOOP_H_

#include <functional>

#include "util/scoped_ptr.h"

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

  void UpdatedDispatch(Dispatch* dispatch);

 private:
  scoped_ptr<EventPoller> poller_;
};

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENTLOOP_H_
