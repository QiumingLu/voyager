#ifndef VOYAGER_PORT_BOUNDED_BLOCKINGQUEUE_H_
#define VOYAGER_PORT_BOUNDED_BLOCKINGQUEUE_H_

#include <deque>
#include <utility>
#include <assert.h>
#include "voyager/port/mutex.h"
#include "voyager/port/mutexlock.h"

namespace voyager {
namespace port {

template<typename T>
class BoundedBlockingQueue {
 public:
  BoundedBlockingQueue(size_t capacity) 
      : mutex_(), 
        not_full(&mutex_),
        not_empty(&mutex_),
        capacity_(capacity)
  { } 

  void Put(const T& t) {
    MutexLock lock(&mutex_);
    while (queue_.size() == capacity_) {
      not_full.Wait();
    }
    assert(queue_.size() < capacity_);
    queue_.push_back(t);
    not_empty.Signal();
  }

  void Put(T&& t) {
    MutexLock lock(&mutex_);
    while (queue_.size() == capacity_) {
      not_full.Wait();
    }
    assert(queue_.size() < capacity_);
    queue_.push_back(std::move(t));
    not_empty.Signal();
 }

  T Take() {
    MutexLock lock(&mutex_);
    while (queue_.empty()) {
      not_empty.Wait();
    }
    assert(!queue_.empty());
    T t(std::move(queue_.front()));
    queue_.pop_front();
    not_full.Signal();
    return t;
  }

  size_t Capacity() const {
    MutexLock lock(&mutex_);
    return capacity_;
  }

  size_t Size() const {
    MutexLock lock(&mutex_);
    return queue_.size();
  }

  bool Empty() const {
    MutexLock lock(&mutex_);
    return queue_.empty();
  }

  bool Full() const {
    MutexLock lock(&mutex_);
    if (queue_.size() == capacity_) {
      return true;
    } 
    return false;
  }

 private:
  mutable Mutex mutex_;
  Condition not_full;
  Condition not_empty;
  const size_t capacity_;
  std::deque<T> queue_;

  // No copying allow
  BoundedBlockingQueue(const BoundedBlockingQueue&);
  void operator=(const BoundedBlockingQueue&);
};

}  // namespace port
}  // namespace voyager

#endif // VOYAGER_PORT_BOUNDED_BLOCKINGQUEUE_H_
