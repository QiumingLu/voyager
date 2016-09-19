#ifndef VOYAGER_CORE_EVENT_POLLER_H_
#define VOYAGER_CORE_EVENT_POLLER_H_

#include <unordered_map>
#include <vector>

#include "voyager/core/dispatch.h"
#include "voyager/core/eventloop.h"

namespace voyager {

class EventPoller {
 public:
  explicit EventPoller(EventLoop* ev);
  virtual ~EventPoller();

  virtual void Poll(int timeout, std::vector<Dispatch*> *dispatches) = 0;
  virtual void RemoveDispatch(Dispatch* dispatch) = 0;
  virtual void UpdateDispatch(Dispatch* dispatch) = 0;
  virtual bool HasDispatch(Dispatch* dispatch) const;

 protected:
  typedef std::unordered_map<int, Dispatch*> DispatchMap;

  DispatchMap dispatch_map_;
  EventLoop* eventloop_;

  // No copying allowed
  EventPoller(const EventPoller&);
  void operator=(const EventPoller&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENT_POLLER_H_
