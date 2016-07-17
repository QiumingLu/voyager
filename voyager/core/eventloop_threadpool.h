#ifndef VOYAGER_CORE_EVENTLOOP_THREADPOOL_H_
#define VOYAGER_CORE_EVENTLOOP_THREADPOOL_H_

#include <string>
#include <vector>

#include "voyager/core/eventloop_thread.h"
#include "voyager/util/scoped_ptr.h"

namespace voyager {

class EventLoop;

class EventLoopThreadPool {
 public:
  EventLoopThreadPool(EventLoop* evloop, 
                      const std::string& poolname, 
                      int size);

  void Start();

  EventLoop* GetNext();

  bool Started() const { return started_; }
  const std::string& PoolName() const { return poolname_; }

 private:
  EventLoop* baseloop_;
  std::string poolname_;
  int size_;
  bool started_;
  size_t next_;
  
  scoped_array<scoped_ptr<EventLoopThread> > threads_;
  std::vector<EventLoop*> eventloops_;

  // No copying allow
  EventLoopThreadPool(const EventLoopThreadPool&);
  void operator=(const EventLoopThreadPool&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENTLOOP_THREADPOOL_H_
