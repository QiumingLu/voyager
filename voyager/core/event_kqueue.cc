#include "voyager/core/event_kqueue.h"

#include <assert.h>
#include <errno.h>
#include <string.h>
#include <poll.h>

#include "voyager/core/dispatch.h"
#include "voyager/util/logging.h"

namespace voyager {

EventKqueue::EventKqueue(EventLoop* ev) 
    : EventPoller(ev),
      kq_(::kqueue()),
      events_(kInitKqueueFdSize) {
  if (kq_ == -1) {
    VOYAGER_LOG(FATAL) << "kqueue: " << strerror(errno);
  }
}

EventKqueue::~EventKqueue() {
}

void EventKqueue::Poll(int timeout, std::vector<Dispatch*>* dispatches) {
  struct timespec out;
  out.tv_sec = timeout / 1000;
  out.tv_nsec = timeout % 1000 * 1000000000;
  int nfds = kevent(kq_, NULL, 0,
                    &*events_.begin(), static_cast<int>(events_.size()),
                    &out);
  if (nfds == -1) {
    VOYAGER_LOG(ERROR) << "kevent: " << strerror(errno);
    return;
  }
  
  for (int i = 0; i < nfds; ++i) {
    Dispatch *dis = reinterpret_cast<Dispatch*>(events_[i].udata);
    int revents = 0;
    if (events_[i].flags & EV_ERROR) {
      revents |= POLLERR;
    }
    if (events_[i].filter == EVFILT_READ) {
      revents |= POLLIN;
    } else if (events_[i].filter == EVFILT_WRITE) {
      revents |= POLLOUT;
    } else {
      revents |= POLLERR;
    }
    dis->SetRevents(revents);
    dispatches->push_back(dis);
  }
  if (nfds == static_cast<int>(events_.size())) {
    events_.resize(events_.size() * 2);
  }
}

void EventKqueue::RemoveDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  int fd = dispatch->Fd();
  assert(dispatch_map_.find(fd) != dispatch_map_.end());
  assert(dispatch_map_[fd] == dispatch);
  assert(dispatch->IsNoneEvent());
  dispatch_map_.erase(fd);
}

void EventKqueue::UpdateDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  int fd = dispatch->Fd();
  std::pair<bool, int> change(dispatch->ChangeEvent());

  if (change.first) {
    if (dispatch_map_.find(fd) != dispatch_map_.end()) {
      dispatch_map_[fd] = dispatch;
    }
    KqueueCTL(EV_ADD | EV_ENABLE, dispatch);
  } else {
    assert(dispatch_map_.find(fd) != dispatch_map_.end());
    KqueueCTL(EV_DELETE, dispatch);
  }
}

void EventKqueue::KqueueCTL(u_short op, Dispatch* dispatch) {
  std::pair<bool, int> change(dispatch->ChangeEvent());
  if (change.second == 0) {
    struct kevent event[2];
    EV_SET(&event[0], dispatch->Fd(), EVFILT_READ, 
           op, 0, 0, reinterpret_cast<void*>(dispatch));
    EV_SET(&event[1], dispatch->Fd(), EVFILT_WRITE, 
           op, 0, 0, reinterpret_cast<void*>(dispatch));
    if (::kevent(kq_, event, 2, NULL, 0, NULL) == -1) { 
      VOYAGER_LOG(ERROR) << "kevent: " << strerror(errno);
    }
  } else {
    struct kevent event;
    short filter = change.second == 1 ? EVFILT_READ : EVFILT_WRITE;
    EV_SET(&event, dispatch->Fd(), filter, 
           op, 0, 0, reinterpret_cast<void*>(dispatch));
    if (::kevent(kq_, &event, 1, NULL, 0, NULL) == -1) { 
      VOYAGER_LOG(ERROR) << "kevent: " << strerror(errno);
    }
  }
}

}  // namespace voyager
