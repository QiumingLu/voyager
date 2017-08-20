// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/event_epoll.h"

#include <string.h>

#include <utility>
#include <vector>

#include "voyager/core/dispatch.h"
#include "voyager/util/logging.h"

namespace voyager {

static const int kNew = -1;
static const int kAdded = 1;
static const int kDeleted = 2;

EventEpoll::EventEpoll(EventLoop* ev)
    : EventPoller(ev),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      epollfds_(kInitEpollFdSize) {
  if (epollfd_ == -1) {
    VOYAGER_LOG(FATAL) << "epoll_create1: " << strerror(errno);
  }
}

EventEpoll::~EventEpoll() { ::close(epollfd_); }

void EventEpoll::Poll(int timeout, std::vector<Dispatch*>* dispatches) {
  int nfds = ::epoll_wait(epollfd_, &*epollfds_.begin(),
                          static_cast<int>(epollfds_.size()), timeout);
  if (nfds == -1) {
    if (errno != EINTR) {
      VOYAGER_LOG(ERROR) << "epoll_wait: " << strerror(errno);
    }
    return;
  }
  for (int i = 0; i < nfds; ++i) {
    Dispatch* dis = reinterpret_cast<Dispatch*>(epollfds_[i].data.ptr);
    dis->SetRevents(epollfds_[i].events);
    dispatches->push_back(dis);
  }
  if (nfds == static_cast<int>(epollfds_.size())) {
    epollfds_.resize(epollfds_.size() * 2);
  }
}

void EventEpoll::RemoveDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  int fd = dispatch->Fd();
  assert(dispatch_map_.find(fd) != dispatch_map_.end());
  assert(dispatch_map_[fd] == dispatch);
  assert(dispatch->IsNoneEvent());
  int idx = dispatch->Index();
  assert(idx == kAdded || idx == kDeleted);
  dispatch_map_.erase(fd);
  if (idx == kAdded) {
    EpollCTL(EPOLL_CTL_DEL, dispatch);
  }
  dispatch->SetIndex(kNew);
}

void EventEpoll::UpdateDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  const int idx = dispatch->Index();
  const int fd = dispatch->Fd();
  if (idx == kNew || idx == kDeleted) {
    if (idx == kNew) {
      assert(dispatch_map_.find(fd) == dispatch_map_.end());
      dispatch_map_.insert(std::make_pair(fd, dispatch));
    } else {
      assert(dispatch_map_.find(fd) != dispatch_map_.end());
      assert(dispatch_map_[fd] == dispatch);
    }
    dispatch->SetIndex(kAdded);
    EpollCTL(EPOLL_CTL_ADD, dispatch);
  } else {
    assert(dispatch_map_.find(fd) != dispatch_map_.end());
    assert(dispatch_map_[fd] == dispatch);
    assert(idx == kAdded);
    if (dispatch->IsNoneEvent()) {
      EpollCTL(EPOLL_CTL_DEL, dispatch);
      dispatch->SetIndex(kDeleted);
    } else {
      EpollCTL(EPOLL_CTL_MOD, dispatch);
    }
  }
}

void EventEpoll::EpollCTL(int op, Dispatch* dispatch) {
  int fd = dispatch->Fd();
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.events = dispatch->Events();
  ev.data.ptr = dispatch;

  if (::epoll_ctl(epollfd_, op, fd, &ev) == -1) {
    VOYAGER_LOG(ERROR) << "epoll_ctl: " << strerror(errno) << " fd=" << fd;
  }
}

}  // namespace voyager
