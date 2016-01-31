#ifndef MIRANTS_CORE_EVENT_POLL_H_
#define MIRANTS_CORE_EVENT_POLL_H_

#include <vector>
#include <sys/poll.h>

#include "core/event_poller.h"

namespace mirants {

class EventPoll : public EventPoller {
 public:
  EventPoll(EventLoop* eventloop);
  virtual ~EventPoll();

  virtual void Poll();
  virtual UpdateDispatch(Dispatch* dispatch);

 private:
  std::vector<struct pollfd> pollfds_;
};

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENT_POLL_H_