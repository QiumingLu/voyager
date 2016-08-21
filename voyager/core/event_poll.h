#ifndef VOYAGER_CORE_EVENT_POLL_H_
#define VOYAGER_CORE_EVENT_POLL_H_

#include "voyager/core/event_poller.h"

namespace voyager {

class EventPoll : public EventPoller {
 public:
  EventPoll(EventLoop* ev);
  virtual ~EventPoll();

  virtual void Poll(int timeout, std::vector<Dispatch*> *dispatches);
  virtual void RemoveDispatch(Dispatch* dispatch);
  virtual void UpdateDispatch(Dispatch* dispatch);

 private:
  std::vector<struct pollfd> pollfds_;
};

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENT_POLL_H_
