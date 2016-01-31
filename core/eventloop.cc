#include "core/eventloop.h"

#include "core/dispatch.h"
#include "core/event_poll.h"

namespace mirants {

EventLoop::EventLoop()
    : poller_(new EventPoll(this)) {
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
  }
}

void EventLoop::RunInLoop(const Func& func) {
  func();
}

void EventLoop::RunInLoop(Func&& func) {
  func();
}

void EventLoop::UpdateDispatch(Dispatch* dispatch) {
  poller_->UpdateDispatch(dispatch);
}

}  // namespace mirants