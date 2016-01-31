#include "core/eventloop.h"

#include "core/dispatch.h"
#include "core/event_poll.h"

namespace mirants {

EventLoop::EventLoop()
    : poller_(new EventPoller()) {
}

EventLoop::~EventLoop() {

}

void EventLoop::Loop() {
  while(true) {
    
  }
}

void EventLoop::RunInLoop(const Func& func) {
  func();
}

void EventLoop::RunInLoop(Func&& func) {
  func();
}

void EventLoop::UpdateDispatch(Dispatch* dispatch) {
  poller_->UpdateDispatch();
}

}  // namespace mirants