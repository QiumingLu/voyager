#include "port/threadpool.h"

#include <algorithm>
#include <utility>

#include <assert.h>
#include <stdio.h>

#include "port/mutexlock.h"

namespace mirants {
namespace port {

ThreadPool::ThreadPool(int poolsize, const std::string& poolname)
    : mutex_(),
      cond_(&mutex_),
      poolsize_(poolsize),
      poolname_(poolname),
      running_(false) 
{ }

ThreadPool::~ThreadPool() {
  if (running_) {
    Stop();
  }
}

void ThreadPool::Start() {
  assert(threads_.empty());
  running_ = true;
  threads_.reserve(poolsize_);
  for (int i = 0; i < poolsize_; ++i) {
    char name[32] = { 0 };
    snprintf(name, sizeof(name), ": thread %d", i+1);
    threads_.push_back(new Thread(
        std::bind(&ThreadPool::ThreadEntry, this), poolname_ + name));
    threads_[i].Start();
  }
}

void ThreadPool::Stop() {
  {
    MutexLock lock(&mutex_);
    running_ = false;
    cond_.Signal();
  }
  using namespace std::placeholders;
  for_each(threads_.begin(), threads_.end(), std::bind(&Thread::Join, _1));
}

void ThreadPool::AddTask(const Task& task) {
  if (threads_.empty()) {
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
  if (threads_.empty()) {
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
    Task t(std::move(TakeTask()));
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
}  // namespace mirants