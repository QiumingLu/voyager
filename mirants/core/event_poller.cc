#include "mirants/core/event_poller.h"

namespace mirants {

EventPoller::EventPoller(EventLoop* ev) : eventloop_(ev) {
}

EventPoller::~EventPoller() { }

bool EventPoller::HasDispatch(Dispatch* dispatch) const {
  eventloop_->AssertThreadSafe();
  std::unordered_map<int, Dispatch*>::const_iterator it;
  for (it = dispatch_map_.begin(); it != dispatch_map_.end(); ++it) {
    if (it->second == dispatch) {
      return true;
    }
  }
  return false;
}

}  // namespace mirants
