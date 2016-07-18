#include "voyager/core/event_kqueue.h"

namespace voyager {

EventKQueue::EventKQueue(EventLoop* ev) : EventPoller(ev) {
}

EventKQueue::~EventKQueue() {
}

void EventKQueue::Poll(int timeout, std::vector<Dispatch*>* dispatches) {
}

void EventKQueue::RemoveDispatch(Dispatch* dispatch) {
}

void EventKQueue::UpdateDispatch(Dispatch* dispatch) {
}

}  // namespace voyager
