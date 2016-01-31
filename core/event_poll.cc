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
        std::map<int, Dispatch*>::const_iterator iter = dispatch_map_.find(it->fd);
        assert(iter != dispatch_map_.end());
        Dispatch* dispatch = iter->second;
        assert(dispatch->Fd() == it->fd);
        dispatch->SetRevents(it->revents);
        dispatches->push_back(dispatch);
      }
    }
  } else if(ret == 0) {
    MIRANTS_LOG(TRACE) << "Nothing Happended";
  } else {
    if (err != EINTR) {
      MIRANTS_LOG(ERROR) << "EventPoll::Poll" << strerror(err);
    }
  }
}

void EventPoll::UpdateDispatch(Dispatch* dispatch) {
  struct pollfd p;
  p.fd = dispatch->Fd();
  p.events = static_cast<short>(dispatch->Events());
  p.revents = 0;
  pollfds_.push_back(p);
  dispatch_map_.insert(std::make_pair(p.fd, dispatch));
}

}  // namespace mirants