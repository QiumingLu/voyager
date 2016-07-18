#include "voyager/core/event_select.h"

namespace voyager {

EventSelect::EventSelect(EventLoop* ev) : EventPoller(ev) {
}

EventSelect::~EventSelect() {
}

void EventSelect::Poll(int timeout, std::vector<Dispatch*>* dispatches) {
}

void EventSelect::RemoveDispatch(Dispatch* dispatch) {
}

void EventSelect::UpdateDispatch(Dispatch* dispatch) {
}

}  // namespace voyager
