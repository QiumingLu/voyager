#ifndef VOYAGER_CORE_EVENT_SELECT_H_
#define VOYAGER_CORE_EVENT_SELECT_H_

#include <sys/select.h>

#include "voyager/core/event_poller.h"

namespace voyager {

class EventSelect : public EventPoller{
 public:
  EventSelect(EventLoop* ev);
  virtual ~EventSelect();
  virtual void Poll(int timeout, std::vector<Dispatch*>* dispatches);
  virtual void RemoveDispatch(Dispatch* dispatch); 
  virtual void UpdateDispatch(Dispatch* dispatch);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENT_SELECT_H_
