#include "mirants/core/dispatch.h"
#include "mirants/core/eventloop.h"
#include "mirants/util/logging.h"

namespace mirants {

Dispatch::Dispatch(EventLoop* eventloop, int fd) 
    : eventloop_(eventloop), 
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      tied_(false),
      event_handling_(false) {

}

Dispatch::~Dispatch() {
  assert(!event_handling_);
}

void Dispatch::EnableRead() {
  events_ |= kReadEvent;
  UpdateEvents();
}

void Dispatch::EnableWrite() {
  events_ |= kWriteEvent;
  UpdateEvents();
}

void Dispatch::DisableRead() {
  events_ &= ~kReadEvent;
  UpdateEvents();
}

void Dispatch::DisableWrite() {
  events_ &= ~kWriteEvent;
  UpdateEvents();
}

void Dispatch::DisableAll() {
  events_ = kNoneEvent;
  UpdateEvents();
}

void Dispatch::UpdateEvents() {
  eventloop_->UpdateDispatch(this);
}

void Dispatch::RemoveEvents() {
  assert(IsNoneEvent());
  eventloop_->RemoveDispatch(this);
}

void Dispatch::HandleEvent() {
  std::shared_ptr<void> guard;
  if (tied_) {
    guard = tie_.lock();
    if (guard) {
      HandleEventWithGuard();
    }
  } else {
    HandleEventWithGuard();
  }
}

void Dispatch::HandleEventWithGuard() {
  event_handling_ = true;
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
  event_handling_ = false;
}

void Dispatch::Tie(const std::shared_ptr<void>& obj) {
  tie_ = obj;
  tied_ = true;
}

}  // namespace mirants
