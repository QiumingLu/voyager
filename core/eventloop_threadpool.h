#ifndef MIRANTS_CORE_EVENTLOOP_THREADPOOL_H_
#define MIRANTS_CORE_EVENTLOOP_THREADPOOL_H_

#include <vector>
#include <boost/ptr_container/ptr_vector.hpp>

namespace mirants {

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool {
 public:
  EventLoopThreadPool(EventLoop* evloop, 
                      const std::string& poolname, 
                      int size);
  ~EventLoopThreadPool();

  void Start();

  EventLoop* GetNext();

  bool Started() const { return started_; }
  const std::string& PoolName() const { return poolname_; }

 private:
  EventLoop* baseloop_;
  std::string poolname_;
  int size_;
  bool started_;
  int next_;
  boost::ptr_vector<EventLoopThread> threads_;
  std::vector<EventLoop*> eventloops_;

  // No copying allow
  EventLoopThreadPool(const EventLoopThreadPool&);
  void operator=(const EventLoopThreadPool&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_EVENTLOOP_THREADPOOL_H_
