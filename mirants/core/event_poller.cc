#include "mirants/core/event_poller.h"

namespace mirants {

EventPoller::EventPoller(EventLoop* ev) : eventloop_(ev) {
}

EventPoller::~EventPoller() { }

}  // namespace mirants
