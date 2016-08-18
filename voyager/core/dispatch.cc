#include "voyager/core/dispatch.h"
#include "voyager/core/eventloop.h"
#include "voyager/util/logging.h"

namespace voyager {

Dispatch::Dispatch(EventLoop* eventloop, int fd) 
    : eventloop_(eventloop), 
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      change_(0),
      tied_(false),
      event_handling_(false) {
}

Dispatch::~Dispatch() {
  assert(!event_handling_);
}

void Dispatch::EnableRead() {
  events_ |= kReadEvent;
  change_ = kEnableRead;
  UpdateEvents();
}

void Dispatch::EnableWrite() {
  events_ |= kWriteEvent;
  change_ = kEnableWrite;
  UpdateEvents();
}

void Dispatch::DisableRead() {
  events_ &= ~kReadEvent;
  change_ = kDisableRead;
  UpdateEvents();
}

void Dispatch::DisableWrite() {
  events_ &= ~kWriteEvent;
  change_ = kDisableWrite;
  UpdateEvents();
}

void Dispatch::DisableAll() {
  if (IsReading() && IsWriting()) { 
    change_ = kDisableAll;
  } else if (IsReading()) {
    change_ = kDisableRead;
  } else if (IsWriting()) {
    change_ = kDisableWrite;
  }
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
    if (close_cb_) { 
      close_cb_(); 
    }
  }
  if (revents_ & POLLNVAL) {
    VOYAGER_LOG(WARN) << "Dispatch::HandleEvent() POLLNVAL";
  }
  if (revents_ & (POLLERR | POLLNVAL)) {
    if (error_cb_) { 
      error_cb_(); 
    }
  }
#ifndef POLLRDHUP
  const int POLLRDHUP = 0;
#endif
  if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
    if (read_cb_) { 
      read_cb_(); 
    }
  }
  if (revents_ & POLLOUT) {
    if (write_cb_) {
      write_cb_();
    }
  }
  event_handling_ = false;
}

void Dispatch::Tie(const std::shared_ptr<void>& obj) {
  tie_ = obj;
  tied_ = true;
}

}  // namespace voyager
