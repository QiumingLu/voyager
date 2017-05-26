// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/event_select.h"

#include <errno.h>
#include <string.h>

#include "voyager/util/logging.h"

namespace voyager {

EventSelect::EventSelect(EventLoop* ev)
    : EventPoller(ev),
      nfds_(0) {
  FD_ZERO(&readfds_);
  FD_ZERO(&writefds_);
  FD_ZERO(&exceptfds_);
}

EventSelect::~EventSelect() {
}

void EventSelect::Poll(int timeout, std::vector<Dispatch*>* dispatches) {
  struct timeval out;
  out.tv_sec = static_cast<time_t>(timeout / 1000);
  out.tv_usec = static_cast<suseconds_t>(timeout % 1000);

  fd_set readfds;
  fd_set writefds;
  fd_set exceptfds;

  FD_ZERO(&readfds);
  FD_ZERO(&writefds);
  FD_ZERO(&exceptfds);

  memcpy(&readfds, &readfds_, sizeof(readfds_));
  memcpy(&writefds, &writefds_, sizeof(writefds_));
  memcpy(&exceptfds, &exceptfds_, sizeof(exceptfds_));

  int ret = ::select(nfds_, &readfds, &writefds, &exceptfds, &out);

  if (ret == -1) {
    if (errno != EINTR) {
      VOYAGER_LOG(ERROR) << "poll: " << strerror(errno);
    }
    return;
  }

  for (std::map<int, Dispatch*>::iterator it = worker_map_.begin();
       it != worker_map_.end(); ++it) {
    int revents = 0;
    if (FD_ISSET(it->first, &readfds)) {
      revents |= POLLIN;
    }
    if (FD_ISSET(it->first, &writefds)) {
      revents |= POLLOUT;
    }
    if (FD_ISSET(it->first, &exceptfds)) {
      revents |= POLLERR;
    }

    if (revents) {
      it->second->SetRevents(revents);
      dispatches->push_back(it->second);
    }
  }
}

void EventSelect::RemoveDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  int fd = dispatch->Fd();
  assert(dispatch_map_.find(fd) != dispatch_map_.end());
  assert(dispatch_map_[fd] == dispatch);
  assert(dispatch->IsNoneEvent());
  dispatch_map_.erase(fd);
  dispatch->SetIndex(-1);
}

void EventSelect::UpdateDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  int fd = dispatch->Fd();
  int modify = dispatch->Modify();

  switch (modify) {
    case Dispatch::kAddRead:
      FD_SET(fd, &readfds_);
      break;

    case Dispatch::kAddWrite:
      FD_SET(fd, &writefds_);
      break;

    case Dispatch::kDeleteRead:
      FD_CLR(fd, &readfds_);
      break;

    case Dispatch::kDeleteWrite:
      FD_CLR(fd, &writefds_);
      break;

    case Dispatch::kEnableWrite:
      FD_SET(fd, &writefds_);
      break;

    case Dispatch::kDisableWrite:
      FD_CLR(fd, &writefds_);
      break;

    case Dispatch::kDeleteAll:
      FD_CLR(fd, &readfds_);
      FD_CLR(fd, &writefds_);
      break;

    case Dispatch::kNoModify:
      break;
  }

  if (dispatch->Index() == -1) {
    assert(dispatch_map_.find(fd) == dispatch_map_.end());
    dispatch_map_[fd] = dispatch;
    dispatch->SetIndex(0);
  } else {
    assert(dispatch_map_.find(fd) != dispatch_map_.end());
  }

  if (dispatch->IsNoneEvent()) {
    if (worker_map_.find(fd) != worker_map_.end()) {
      worker_map_.erase(fd);
    }
  } else {
    worker_map_[fd] = dispatch;
  }

  if (worker_map_.empty()) {
    nfds_ = 1;
  } else {
    nfds_ = worker_map_.rbegin()->first + 1;
  }
}

}  // namespace voyager
