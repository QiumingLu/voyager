#include "core/eventloop.h"
#include "core/dispatch.h"
#include "core/event_poll.h"
#include "port/mutexlock.h"
#include "util/logging.h"

namespace mirants {

EventLoop::EventLoop()
    : tid_(port::CurrentThread::Tid()),
      poller_(new EventPoll(this)) {
}

EventLoop::~EventLoop() {

}

void EventLoop::Loop() {
  while(true) {
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

void EventLoop::UpdateDispatch(Dispatch* dispatch) {
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
