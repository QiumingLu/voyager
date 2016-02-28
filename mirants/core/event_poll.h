#ifndef MIRANTS_CORE_EVENT_POLL_H_
#define MIRANTS_CORE_EVENT_POLL_H_

#include <vector>
#include <sys/poll.h>

#include "mirants/core/event_poller.h"

namespace mirants {

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

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENT_POLL_H_
