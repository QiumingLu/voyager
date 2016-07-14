#ifndef MIRANTS_CORE_EVENT_EPOLL_H_
#define MIRANTS_CORE_EVENT_EPOLL_H_

#ifdef __linux__
#include <sys/epoll.h>
#endif

#include "mirants/core/event_poller.h"

namespace mirants {

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

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENT_EPOLL_H_
