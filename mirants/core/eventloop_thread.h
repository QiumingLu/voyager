#ifndef MIRANTS_CORE_EVENTLOOP_THREAD_H_
#define MIRANTS_CORE_EVENTLOOP_THREAD_H_

#include "mirants/port/mutex.h"
#include "mirants/port/thread.h"

namespace mirants {

class EventLoop;

class EventLoopThread {
 public:
  EventLoopThread(const std::string& name = std::string());
  ~EventLoopThread();

  EventLoop* StartLoop();

 private:
  void ThreadFunc();

  EventLoop *eventloop_;
  port::Mutex mu_;
  port::Condition cond_;
  port::Thread thread_;

  // No copying allow
  EventLoopThread(const EventLoopThread&);
  void operator=(const EventLoopThread&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENTLOOP_THREAD_H_
