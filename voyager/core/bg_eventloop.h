#ifndef VOYAGER_CORE_BG_EVENTLOOP_H_
#define VOYAGER_CORE_BG_EVENTLOOP_H_

#include "voyager/port/mutex.h"
#include "voyager/port/thread.h"

namespace voyager {

class EventLoop;

class BGEventLoop {
 public:
  BGEventLoop(const std::string& name = std::string());
  ~BGEventLoop();

  EventLoop* Loop();

 private:
  void ThreadFunc();

  EventLoop *eventloop_;
  port::Mutex mu_;
  port::Condition cond_;
  port::Thread thread_;

  // No copying allow
  BGEventLoop(const BGEventLoop&);
  void operator=(const BGEventLoop&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_BG_EVENTLOOP_H_
