#ifndef VOYAGER_PORT_THREADPOOL_H_
#define VOYAGER_PORT_THREADPOOL_H_

#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "voyager/port/mutex.h"
#include "voyager/port/thread.h"

namespace voyager {
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
  const int poolsize_;
  bool running_;
  std::string poolname_;
  std::vector<std::unique_ptr<Thread>> threads_;
  std::deque<Task> tasks_;

  // No copying allow
  ThreadPool(const ThreadPool&);
  void operator=(const ThreadPool&);
};

}  // namespace port
}  // namespace voyager

#endif  // VOYAGER_PORT_THREADPOOL_H_
