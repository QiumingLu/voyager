// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_EVENT_KQUEUE_H_
#define VOYAGER_CORE_EVENT_KQUEUE_H_

#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>

#include <vector>

#include "voyager/core/event_poller.h"

namespace voyager {

class EventKqueue : public EventPoller {
 public:
  explicit EventKqueue(EventLoop* ev);
  virtual ~EventKqueue();

  virtual void Poll(int timeout, std::vector<Dispatch*>* dispatches);
  virtual void RemoveDispatch(Dispatch* dispatch);
  virtual void UpdateDispatch(Dispatch* dispatch);

 private:
  static const size_t kInitKqueueFdSize = 16;

  const int kq_;
  std::vector<struct kevent> events_;
};

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENT_KQUEUE_H_
