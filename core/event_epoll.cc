#include "core/event_epoll.h"
#include "core/dispatch.h"

namespace mirants {

EventEpoll::EventEpoll(EventLoop* eventloop) : EventPoller(eventloop) {
}

EventEpoll::~EventEpoll() {
}

void EventEpoll::Poll(int timeout, std::vector<Dispatch*> *dispatch) {
}

void EventEpoll::RemoveDispatch(Dispatch* dispatch) {
}

void EventEpoll::UpdateDispatch(Dispatch* dispatch) {
}

}  // namespace mirants
