#ifndef MIRANTS_CORE_EVENT_EPOLL_H_
#define MIRANTS_CORE_EVENT_EPOLL_H_

#include <vector>
#include <sys/epoll.h>

#include "core/event_poller.h"

namespace mirants {

class EventEpoll : public EventPoller{
 public:
  EventEpoll();
  virtual ~EventEpoll();
  virtual void Poll();
  virtual void UpdateDispatch();

 private:
  std::vector<struct epoll_event> epollfds_;
};

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENT_EPOLL_H_