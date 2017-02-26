// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/event_poller.h"

namespace voyager {

EventPoller::EventPoller(EventLoop* ev) : eventloop_(ev) {
}

EventPoller::~EventPoller() { }

bool EventPoller::HasDispatch(Dispatch* dispatch) const {
  eventloop_->AssertInMyLoop();
  DispatchMap::const_iterator it;
  for (it = dispatch_map_.begin(); it != dispatch_map_.end(); ++it) {
    if (it->second == dispatch) {
      return true;
    }
  }
  return false;
}

}  // namespace voyager
