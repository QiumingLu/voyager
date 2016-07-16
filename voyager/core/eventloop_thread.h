#ifndef VOYAGER_CORE_EVENTLOOP_THREAD_H_
#define VOYAGER_CORE_EVENTLOOP_THREAD_H_

#include "voyager/port/mutex.h"
#include "voyager/port/thread.h"

namespace voyager {

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

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENTLOOP_THREAD_H_
