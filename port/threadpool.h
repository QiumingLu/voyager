#ifndef MIRANTS_PORT_THREADPOOL_H_
#define MIRANTS_PORT_THREADPOOL_H_

#include <functional>
#include <string>
#include <boost/ptr_container/ptr_vector.hpp>
#include <deque>
#include "port/mutex.h"
#include "port/thread.h"

namespace mirants {
namespace port {

class ThreadPool {
 public:
  typedef std::function<void ()> Task;

  explicit ThreadPool(int poolsize,
                      const std::string& poolname = std::string("ThreadPool"));
  ~ThreadPool();

  void Start();
  void Stop();

  void AddTask(const Task& task);
  void AddTask(Task&& task);

  const std::string& PoolName() const { return poolname_; }
  size_t TaskSize() const;

 private:
  void ThreadEntry();
  Task TakeTask();


  mutable Mutex mutex_;
  Condition cond_;
  int poolsize_;
  std::string poolname_;
  boost::ptr_vector<Thread> threads_;
  std::deque<Task> tasks_;
  bool running_;

  // No copying allow
  ThreadPool(const ThreadPool&);
  void operator=(const ThreadPool&);
};

}  // namespace port
}  // namespace mirants

#endif  // MIRANTS_PORT_THREADPOOL_H_
