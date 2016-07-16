#ifndef VOYAGER_CORE_EVENT_EPOLL_H_
#define VOYAGER_CORE_EVENT_EPOLL_H_

#ifdef __linux__
#include <sys/epoll.h>
#endif

#include "voyager/core/event_poller.h"

namespace voyager {

#ifdef __linux__

class EventEpoll : public EventPoller {
 public:
  EventEpoll(EventLoop* ev);
  virtual ~EventEpoll();

  virtual void Poll(int timeout, std::vector<Dispatch*> *dispatches);
  virtual void RemoveDispatch(Dispatch* dispatch);
  virtual void UpdateDispatch(Dispatch* dispatch);

 private:
  static const int kInitEpollFdSize = 16;

  void EpollCTL(int op, Dispatch* dispatch);

  int epollfd_;
  std::vector<struct epoll_event> epollfds_;
};

#endif

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENT_EPOLL_H_
