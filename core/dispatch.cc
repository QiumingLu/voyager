#include "core/dispatch.h"
#include "core/eventloop.h"
#include "util/logging.h"

namespace mirants {

Dispatch::Dispatch(EventLoop* eventloop, int fd) 
    : eventloop_(eventloop), fd_(fd) {

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

void Dispatch::HandleEvent() {
  if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
    if (closefunc_) { 
      closefunc_(); 
    }
  }
  if (revents_ & POLLNVAL) {
    MIRANTS_LOG(WARN) << "Dispatch::HandleEvent() POLLNVAL";
  }
  if (revents_ & (POLLERR | POLLNVAL)) {
    if (errorfunc_) { 
      errorfunc_(); 
    }
  }
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (readfunc_) { 
      readfunc_(); 
    }
  }
  if (revents_ & POLLOUT) {
    if (writefunc_) {
      writefunc_();
    }
  }
}

}  // namespace mirants