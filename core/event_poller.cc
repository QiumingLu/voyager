#include "core/event_poller.h"

namespace mirants {

EventPoller::EventPoller(EventLoop* eventloop) : eventloop_(eventloop) {
}

EventPoller::~EventPoller() { }

}  // namespace mirants