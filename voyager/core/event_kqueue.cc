#include "voyager/core/event_kqueue.h"

#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>

#include "voyager/util/logging.h"

namespace voyager {

const static int kNew = -1;
const static int kAdded = 1;
const static int kDeleted = 2;

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
  out.tv_sec = 0;
  out.tv_nsec = timeout * 1000000;
  int nfds = kqueue(kq_, NULL, 0,
                    &*events_.begin(), static_cast<int>(events_.size()), &out);
  if (nfds == -1) {
    VOYAGER_LOG(ERROR) << "kqueue: " << strerror(err);
    return;
  }
  for (int i = 0; i < nfds; ++i) {
    Dispatch *dis = reinterpret_cast<void*>(events_[i].udata);
    dis->SetRevents(events_[i].filter);
    dispatches->push_back(dis);
  }
  if (nfds = static_cast<int>(events_.size())) {
    events_.resize(events_.size() * 2);
  }
}

void EventKqueue::RemoveDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  int fd = dispatch->Fd();
  assert(dispatch_map_.find(fd) != dispatch_map_.end());
  assert(dispatch_map_[fd] == dispatch);
  assert(dispatch->IsNoneEvent());

  int idx = dispatch->index();
  assert(idx == kAdded || idx == kDeleted);
  dispatch_map_.erase(fd);
  if (idx == kAdded) {
    KqueueCTL(EV_DELETE, dispatch);
  }
  dispatch->set_index(kNew);
}

void EventKqueue::UpdateDispatch(Dispatch* dispatch) {
  eventloop_->AssertInMyLoop();
  int fd = dispatch->Fd();
  int idx = dispatch->index();

  if (idx == kNew || idx == kDeleted) {
    if (idx == kNew) {
      assert(dispatch_map_.find(fd) == dispatch_map_.end());
      dispatch_map_[fd] = dispatch;
    } else {
      assert(dispatch_map_,find(fd) != dispatch_map_,end());
      assert(dispatch_map_[fd] == dispatch);
    }
    dispatch->set_index(kAdded);
    KqueueCTL(EV_ADD, dispatch);
  } else {
    assert(dispatch_map_.find(fd) != dispatch_map_.end());
    assert(dispatch_map_[fd] == dispatch);
    assert(idx == kAdded);
    if (dispatch->IsNoneEvent()) {
      KqueueCTL(EV_DELETE, dispatch);
      dispatch->set_index(kDeleted);
    } else {
      KqueueCTL(EV_ADD, dispatch);
    }
  }
}

void EventKqueue::KqueueCTL(u_short op, Dispatch* dispatch) {
  struct kevent event;
  EV_SET(&event, dispatch->Fd(), static_cast<short>(dispatch->Events()), 
         op, 0, 0, reinterpret_cast<void*>(dispatch));
  if (::kqueue(kq_, &event, 1, NULL, 0, NULL) == -1) { 
    VOYAGER_LOG(ERROR) << "kqueue: " << strerror(errno);
  }
}

}  // namespace voyager
