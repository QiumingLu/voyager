#ifndef MIRANTS_CORE_EVENT_POLLER_H_
#define MIRANTS_CORE_EVENT_POLLER_H_

#include <unordered_map>
#include <vector>

#include "core/eventloop.h"

namespace mirants {

class Dispatch;

class EventPoller {
 public:
  EventPoller(EventLoop* ev);
  virtual ~EventPoller();

  virtual void Poll(int timeout, std::vector<Dispatch*> *dispatches) = 0;
  virtual void RemoveDispatch(Dispatch* dispatch) = 0;
  virtual void UpdateDispatch(Dispatch* dispatch) = 0;

 protected:
  std::unordered_map<int, Dispatch*> dispatch_map_;
  EventLoop* eventloop_;

  // No copying allow
  EventPoller(const EventPoller&);
  void operator=(const EventPoller&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENT_POLLER_H_
