#include "core/event_poll.h"
#include <string.h>
#include "core/dispatch.h"
#include "util/logging.h"


namespace mirants {

EventPoll::EventPoll(EventLoop* eventloop) : EventPoller(eventloop) {
}

EventPoll::~EventPoll() {
}

void EventPoll::Poll(int timeout, std::vector<Dispatch*> *dispatches) {
  int ret = ::poll(&(*pollfds_.begin()), pollfds_.size(), timeout);
  int err = errno;
  if(ret > 0) {
    for (std::vector<pollfd>::const_iterator it = pollfds_.begin(); 
        it != pollfds_.end() && ret > 0; ++it) {
      if(it->revents > 0) {
        --ret;
        std::unordered_map<int, Dispatch*>::iterator iter = dispatch_map_.find(it->fd);
        assert(iter != dispatch_map_.end());
        Dispatch* dispatch = iter->second;
        assert(dispatch->Fd() == it->fd);
        dispatch->SetRevents(it->revents);
        dispatches->push_back(dispatch);
      }
    }
  } else if(ret == 0) {
    // MIRANTS_LOG(TRACE) << "Nothing Happended";
  } else {
    if (err != EINTR) {
      MIRANTS_LOG(ERROR) << "EventPoll::Poll" << strerror(err);
    }
  }
}

void EventPoll::RemoveDispatch(Dispatch* dispatch) {
  eventloop_->AssertThreadSafe();
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
    std::swap(pollfds_[idx], pollfds_.back());
    if (id < 0) {
      id = -id-1;
    }
    dispatch_map_[id]->set_index(idx);
    pollfds_.pop_back();
  }
}

void EventPoll::UpdateDispatch(Dispatch* dispatch) {
  eventloop_->AssertThreadSafe();
  if (dispatch->index() == -1) {
    assert(dispatch_map_.find(dispatch->Fd()) == dispatch_map_.end());
    struct pollfd p;
    p.fd = dispatch->Fd();
    p.events = static_cast<short>(dispatch->Events());
    p.revents = 0;
    pollfds_.push_back(p);
    dispatch->set_index(static_cast<int>(pollfds_.size()) - 1);
    dispatch_map_[p.fd] = dispatch;
    std::unordered_map<int, Dispatch*>::iterator iter = dispatch_map_.find(p.fd);
  } else {
    assert(dispatch_map_.find(dispatch->Fd()) != dispatch_map_.end());
    assert(dispatch_map_[dispatch->Fd()] == dispatch);
    int idx = dispatch->index();
    assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
    assert(pollfds_[idx].fd == dispatch->Fd() || 
           pollfds_[idx].fd == -dispatch->Fd()-1);
    pollfds_[idx].events = static_cast<short>(dispatch->Events());
    pollfds_[idx].revents = 0;
    if (dispatch->IsNoneEvent()) {
      // ignore this pollfd
      pollfds_[idx].fd = -dispatch->Fd() - 1;
    }
  }
}

}  // namespace mirants
