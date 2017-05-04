// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/event_poll.h"

#include <string.h>
#include <errno.h>

#include <utility>

#include "voyager/core/dispatch.h"
#include "voyager/util/logging.h"

namespace voyager {

EventPoll::EventPoll(EventLoop* ev) : EventPoller(ev) {
}

EventPoll::~EventPoll() {
}

void EventPoll::Poll(int timeout, std::vector<Dispatch*> *dispatches) {
  int ret = ::poll(&(*pollfds_.begin()),
                   static_cast<nfds_t>(pollfds_.size()), timeout);
  if (ret == -1) {
    if (errno != EINTR) {
      VOYAGER_LOG(ERROR) << "poll: " << strerror(errno);
    }
    return;
  }
  for (std::vector<pollfd>::const_iterator it = pollfds_.begin();
       it != pollfds_.end() && ret > 0; ++it) {
    if (it->revents > 0) {
      --ret;
      DispatchMap::iterator iter = dispatch_map_.find(it->fd);
      assert(iter != dispatch_map_.end());
      Dispatch* dispatch = iter->second;
      assert(dispatch->Fd() == it->fd);
      dispatch->SetRevents(it->revents);
      dispatches->push_back(dispatch);
    }
  }
}

void EventPoll::RemoveDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  assert(dispatch_map_.find(dispatch->Fd()) != dispatch_map_.end());
  assert(dispatch_map_[dispatch->Fd()] == dispatch);
  assert(dispatch->IsNoneEvent());

  int idx = dispatch->index();
  assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
  assert(pollfds_[idx].fd == -dispatch->Fd()-1 &&
         pollfds_[idx].events == dispatch->Events());
  dispatch_map_.erase(dispatch->Fd());
  if (idx == static_cast<int>(pollfds_.size())-1) {
    pollfds_.pop_back();
  } else {
    int id = pollfds_.back().fd;
    std::swap(pollfds_[static_cast<size_t>(idx)], pollfds_.back());
    if (id < 0) {
      id = -id-1;
    }
    dispatch_map_[id]->SetIndex(idx);
    pollfds_.pop_back();
  }
}

void EventPoll::UpdateDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  if (dispatch->index() == -1) {
    assert(dispatch_map_.find(dispatch->Fd()) == dispatch_map_.end());
    struct pollfd p;
    p.fd = dispatch->Fd();
    p.events = static_cast<short>(dispatch->Events());
    p.revents = 0;
    pollfds_.push_back(p);
    dispatch->SetIndex(static_cast<int>(pollfds_.size()) - 1);
    dispatch_map_[p.fd] = dispatch;
  } else {
    assert(dispatch_map_.find(dispatch->Fd()) != dispatch_map_.end());
    assert(dispatch_map_[dispatch->Fd()] == dispatch);
    int idx = dispatch->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    assert(pollfds_[idx].fd == dispatch->Fd() ||
           pollfds_[idx].fd == -dispatch->Fd()-1);
    pollfds_[static_cast<size_t>(idx)].events
        = static_cast<short>(dispatch->Events());
    pollfds_[static_cast<size_t>(idx)].revents = 0;
    if (dispatch->IsNoneEvent()) {
      // ignore this pollfd
      pollfds_[static_cast<size_t>(idx)].fd = -dispatch->Fd() - 1;
    }
  }
}

}  // namespace voyager
