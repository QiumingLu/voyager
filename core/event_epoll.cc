#include "core/event_epoll.h"

#include <utility>
#include <string.h>
#include "core/dispatch.h"
#include "core/socket_util.h"
#include "util/logging.h"

namespace {
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
}

namespace mirants {

EventEpoll::EventEpoll(EventLoop* ev) 
    : EventPoller(ev),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      epollfds_(kInitEpollFdSize) {
  if (epollfd_ == -1) {
    MIRANTS_LOG(FATAL) << "epoll_create1: " << strerror(errno);
  }
}

EventEpoll::~EventEpoll() {
  sockets::CloseFd(epollfd_);
}

void EventEpoll::Poll(int timeout, std::vector<Dispatch*> *dispatches) {
  int nfds = ::epoll_wait(epollfd_, 
                       &*epollfds_.begin(), 
                       static_cast<int>(epollfds_.size()),
                       timeout);
  int err = errno;
  if (nfds == -1) {
    if (err != EINTR) {
      MIRANTS_LOG(ERROR) << "epoll_wait: " << strerror(err);
    }
    return;
  }
  for (int i = 0; i < nfds; ++i) {
    Dispatch* dis = static_cast<Dispatch*>(epollfds_[i].data.ptr);
    dis->SetRevents(epollfds_[i].events);
    dispatches->push_back(dis);
  }
  if (nfds == static_cast<int>(epollfds_.size())) {
    epollfds_.resize(epollfds_.size()*2);
  }
}

void EventEpoll::RemoveDispatch(Dispatch* dispatch) {
  eventloop_->AssertThreadSafe();
  int fd = dispatch->Fd();
  assert(dispatch_map_.find(fd) != dispatch_map_.end());
  assert(dispatch_map_[fd] == dispatch);
  assert(dispatch->IsNoneEvent());
  int idx = dispatch->index();
  assert(idx == kAdded || idx == kDeleted);
  dispatch_map_.erase(fd);
  if (idx == kAdded) {
    EpollCTL(EPOLL_CTL_DEL, dispatch);
  }
  dispatch->set_index(kNew);
}

void EventEpoll::UpdateDispatch(Dispatch* dispatch) {
  eventloop_->AssertThreadSafe();
  const int idx = dispatch->index();
  const int fd = dispatch->Fd();
  if (idx == kNew || idx == kDeleted) {
    if (idx == kNew) {
      assert(dispatch_map_.find(fd) == dispatch_map_.end());
      dispatch_map_.insert(std::make_pair(fd, dispatch));
    } else {
      assert(dispatch_map_.find(fd) != dispatch_map_.end());
      assert(dispatch_map_[fd] == dispatch);
    }
    dispatch->set_index(kAdded);
    EpollCTL(EPOLL_CTL_ADD, dispatch);
  } else {
    assert(dispatch_map_.find(fd)  != dispatch_map_.end());
    assert(dispatch_map_[fd] = dispatch);
    assert(idx == kAdded);
    if (dispatch->IsNoneEvent()) {
      EpollCTL(EPOLL_CTL_DEL, dispatch);
      dispatch->set_index(kDeleted);
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
    MIRANTS_LOG(ERROR) << "epoll_wait: " << strerror(errno) << " fd=" << fd;
  }
}

}  // namespace mirants
