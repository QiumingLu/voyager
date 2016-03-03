#include "mirants/core/eventloop.h"

#include <signal.h>

#include "mirants/core/dispatch.h"
#include "mirants/core/event_epoll.h"
#include "mirants/port/mutexlock.h"
#include "mirants/util/logging.h"
#include "mirants/util/timestamp.h"

namespace mirants {

class IgnoreSIGPIPE {
 public:
  IgnoreSIGPIPE() {
    ::signal(SIGPIPE, SIG_IGN);
  }
};

IgnoreSIGPIPE ignore_SIGPIPE;

EventLoop::EventLoop()
    : tid_(port::CurrentThread::Tid()),
      poller_(new EventEpoll(this)),
      timer_ev_(new TimerEvent(this)),
      exit_(false) {
}

EventLoop::~EventLoop() {
  MIRANTS_LOG(INFO) << "EventLoop::~EventLoop";
}

void EventLoop::Loop() {
  while(!exit_) {
    std::vector<Dispatch*> dispatches;
    poller_->Poll(2330, &dispatches);
    for (std::vector<Dispatch*>::iterator it = dispatches.begin();
        it != dispatches.end(); ++it) {
      (*it)->HandleEvent();
    }
    RunFuncQueue();
  }
}

void EventLoop::RunInLoop(const Func& func) {
  if (IsInCreatedThread()) {
    func();
  } else {
    QueueInLoop(func);
  }
}

void EventLoop::RunInLoop(Func&& func) {
  if (IsInCreatedThread()) {
    func();
  } else {
    QueueInLoop(std::move(func));
  }
}

void EventLoop::QueueInLoop(const Func& func) {
  {
    port::MutexLock lock(&mu_);
    funcqueue_.push_back(func);
  }
}

void EventLoop::QueueInLoop(Func&& func) {
  {
    port::MutexLock lock(&mu_);
    funcqueue_.push_back(std::move(func));
  }
}

Timer* EventLoop::RunAt(const Timestamp& t, const TimeProcCallback& timeproc) {
  return timer_ev_->AddTimer(timeproc, t, 0.0);
}

Timer* EventLoop::RunAfter(double delay, const TimeProcCallback& timeproc) {
  Timestamp t(AddTime(Timestamp::Now(), delay));
  return RunAt(t, timeproc);
}

Timer* EventLoop::RunEvery(double interval, const TimeProcCallback& timeproc) {
  Timestamp t(AddTime(Timestamp::Now(), interval));
  return timer_ev_->AddTimer(timeproc, t, interval);
}

Timer* EventLoop::RunAt(const Timestamp& t, TimeProcCallback&& timeproc) {
  return timer_ev_->AddTimer(std::move(timeproc), t, 0.0);
}

Timer* EventLoop::RunAfter(double delay, TimeProcCallback&& timeproc) {
  Timestamp t(AddTime(Timestamp::Now(), delay));
  return RunAt(t, std::move(timeproc));
}

Timer* EventLoop::RunEvery(double interval, TimeProcCallback&& timeproc) {
  Timestamp t(AddTime(Timestamp::Now(), interval));
  return timer_ev_->AddTimer(timeproc, t, interval);
}

void EventLoop::DeleteTimer(Timer* t) {
  timer_ev_->DeleteTimer(t);
}

void EventLoop::RemoveDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  this->AssertThreadSafe();
  poller_->RemoveDispatch(dispatch);
}

void EventLoop::UpdateDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  this->AssertThreadSafe();
  poller_->UpdateDispatch(dispatch);
}

void EventLoop::RunFuncQueue() {
  std::vector<Func> funcs;
  {
    port::MutexLock lock(&mu_);
    funcs.swap(funcqueue_);
  }
  for (std::vector<Func>::iterator it = funcs.begin(); 
       it != funcs.end(); ++it) {
    (*it)();
  }
}

void EventLoop::AbortForNotInCreatedThread() {
  MIRANTS_LOG(FATAL) << "EventLoop::AbortForNotInCreatedThread - (EventLoop)"
                     << this << " was not created in tid_ = " << tid_
                     << ", currentthread's tid_ = " 
                     << port::CurrentThread::Tid();
}

}  // namespace mirants
