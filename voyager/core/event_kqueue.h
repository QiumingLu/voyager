#ifndef VOYAGER_CORE_EVENT_KQUEUE_H_
#define VOYAGER_CORE_EVENT_KQUEUE_H_

#include "voyager/core/event_poller.h"

namespace voyager {
  
class EventKQueue : public EventPoller {
 public:
  EventKQueue(EventLoop* ev);
  virtual ~EventKQueue();

  virtual void Poll(int timeout, std::vector<Dispatch*>* dispatches);
  virtual void RemoveDispatch(Dispatch* dispatch);
  virtual void UpdateDispatch(Dispatch* dispatch);
};

}  // namespace voyager

#endif // VOYAGER_CORE_EVENT_KQUEUE_H_
