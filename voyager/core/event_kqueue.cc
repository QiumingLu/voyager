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
  int nfds = ::kevent(kq_, NULL, 0,
                      &*events_.begin(), static_cast<int>(events_.size()),
                      &out);
  if (nfds == -1) {
    VOYAGER_LOG(ERROR) << "kevent: " << strerror(errno);
  }
  
  for (int i = 0; i < nfds; ++i) {
    Dispatch *dis = reinterpret_cast<Dispatch*>(events_[i].udata);
    int revents = 0;
    if (events_[i].flags & EV_ERROR) {
      revents |= POLLERR;
    }
    if (events_[i].filter == EVFILT_READ) {
      revents |= POLLIN;
    } 
    if (events_[i].filter == EVFILT_WRITE) {
      revents |= POLLOUT;
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
  int change = dispatch->ChangeEvent();
  int result;

  switch(change) {
    case kEnableRead: {
      result = (fd, EVFILT_READ, EV_ADD | EV_ENABLE);
      break;
    }

    case kEnableWrite: {
      result = (fd, EVFILT_WRITE, EV_ADD | EV_ENABLE)
      break;
    }

    case kDisableRead: {
      assert(dispatch_map_.find(fd) != dispatch_map_.end());
      result = KqueueCTL(fd, EVFILT_READ, EV_DELETE);
      break;
    }

    case kDisableWrite: {
      assert(dispatch_map_.find(fd) != dispatch_map_.end());
      result = KqueueCTL(fd, EVFILT_WRITE, EV_DELETE);      
      break;
    }

    case kDisableAll: {
      struct kevent event[2];
      EV_SET(&event[0], fd, EVFILT_READ, EV_DELETE
             0, 0, reinterpret_cast<void*>(dispatch));
      EV_SET(&event[1], fd, EVFILT_WRITE, EV_DELETE
             0, 0, reinterpret_cast<void*>(dispatch));
      result = ::kevent(kq_, event, 2, NULL, 0, NULL);
      break;
    }

    if (result == -1) { 
      VOYAGER_LOG(ERROR) << "kevent: " << strerror(errno);
    } 
    
    dispatch_map_[fd] = dispatch;
  }
}

int EventKqueue::KqueueCTL(int ident, short filter, u_short flags) {
  struct kevent event;
  EV_SET(&event, ident, filter, flags, 
         0, 0, reinterpret_cast<void*>(dispatch));
  return ::kevent(kq_, &event, 1, NULL, 0, NULL);
}

}  // namespace voyager
