#ifndef VOYAGER_CORE_EVENT_EPOLL_H_
#define VOYAGER_CORE_EVENT_EPOLL_H_

#include <sys/epoll.h>

#include <vector>

#include "voyager/core/event_poller.h"

namespace voyager {

class EventEpoll : public EventPoller {
 public:
  explicit EventEpoll(EventLoop* ev);
  virtual ~EventEpoll();

  virtual void Poll(int timeout, std::vector<Dispatch*>* dispatches);
  virtual void RemoveDispatch(Dispatch* dispatch);
  virtual void UpdateDispatch(Dispatch* dispatch);

 private:
  static const size_t kInitEpollFdSize = 16;

  void EpollCTL(int op, Dispatch* dispatch);

  int epollfd_;
  std::vector<struct epoll_event> epollfds_;
};

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENT_EPOLL_H_
