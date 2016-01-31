#include "core/dispatch.h"
#include "core/eventloop.h"

namespace mirants {

Dispatch::Dispatch(EventLoop* eventloop) : eventloop_(eventloop), fd_(fd) {

}

Dispatch::~Dispatch() {

}

void Dispatch::EnableRead() {
  events_ |= kReadEvent;
  UpdateEvent();
}

void Dispatch::EnableWrite() {
  events_ |= kWriteEvent;
  UpdateEvent();
}

void Dispatch::DisableRead() {
  events_ &= ~kReadEvent;
  UpdateEvent();
}

void Dispatch::DisableWrite() {
  events_ &= ~kWriteEvent;
  UpdateEvent();
}

void Dispatch::DisableAll() {
  events_ = kNoneEvent;
  UpdateEvent();
}

void Dispatch::UpdateEvent() {
  eventloop_->UpdateDispatch(this);
}

}  // namespace mirants