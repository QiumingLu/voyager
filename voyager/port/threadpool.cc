#include "voyager/port/threadpool.h"

#include <algorithm>
#include <utility>

#include <assert.h>

#include "voyager/port/mutexlock.h"
#include "voyager/util/logging.h"

namespace voyager {
namespace port {

ThreadPool::ThreadPool(int poolsize, const std::string& poolname)
    : mutex_(),
      cond_(&mutex_),
      poolsize_(poolsize),
      poolname_(poolname),
      threads_(new scoped_ptr<Thread>[poolsize]),
      tasks_(),
      running_(false) 
{ }

ThreadPool::~ThreadPool() {
  if (running_) {
    Stop();
  }
}

void ThreadPool::Start() {
  assert(!running_);
  running_ = true;
  for (int i = 0; i < poolsize_; ++i) {
    char name[32] = { 0 };
    snprintf(name, sizeof(name), ": thread %d", i+1);
    threads_[i].reset(new Thread(
        std::bind(&ThreadPool::ThreadEntry, this), poolname_ + name));
    threads_[i]->Start();
  }
}

void ThreadPool::Stop() {
  {
    MutexLock lock(&mutex_);
    running_ = false;
    cond_.SignalAll();
  }
  using namespace std::placeholders;
  for (int i = 0; i < poolsize_; ++i) {
    threads_[i]->Join();
  }
}

void ThreadPool::AddTask(const Task& task) {
  if (poolsize_ == 0) {
        VOYAGER_LOG(INFO) << "AddTask1"; 
    task();
  } else {
    MutexLock lock(&mutex_);
    if (running_) {
      tasks_.push_back(task);
      cond_.Signal();
    }
  }
} 

void ThreadPool::AddTask(Task&& task) {
  if (poolsize_ == 0) {
    VOYAGER_LOG(INFO) << "AddTask"; 
    task();
  } else {
    MutexLock lock(&mutex_);
    if (running_) {
      tasks_.push_back(std::move(task));
      cond_.Signal();
    }
  }
}

ThreadPool::Task ThreadPool::TakeTask() {
  MutexLock lock(&mutex_);
  while (tasks_.empty() && running_) {
    cond_.Wait();
  }
  Task t;
  if (!tasks_.empty()) {
    t = std::move(tasks_.front());
    tasks_.pop_front();
  }
  return t;
}

void ThreadPool::ThreadEntry() {
  while (running_) {
    Task t(TakeTask());
    if (t) {
      t();
    }
  }
}

size_t ThreadPool::TaskSize() const {
  MutexLock lock(&mutex_);
  return tasks_.size();
}

}  // namespace port
}  // namespace voyager
