#ifndef VOYAGER_PORT_BLOCKINGQUEUE_H_
#define VOYAGER_PORT_BLOCKINGQUEUE_H_

#include <assert.h>

#include <deque>
#include <utility>

#include "voyager/port/mutex.h"
#include "voyager/port/mutexlock.h"

namespace voyager {
namespace port {

template<typename T>
class BlockingQueue {
 public:
  BlockingQueue() : mutex_(), not_empty(&mutex_) { }

  void Put(const T& t) {
    MutexLock lock(&mutex_);
    queue_.push_back(t);
    assert(queue_.size() > 0);
    not_empty.Signal();
  }

  void Put(T&& t) {
    MutexLock lock(&mutex_);
    queue_.push_back(std::move(t));
    assert(queue_.size() > 0);
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
    return t;
  }

  size_t Size() const {
    MutexLock lock(&mutex_);
    return queue_.size();
  }

  bool Empty() const {
    MutexLock lock(&mutex_);
    return queue_.empty();
  }

 private:
  mutable Mutex mutex_;
  Condition not_empty;
  std::deque<T> queue_;

  // No copying allowed
  BlockingQueue(const BlockingQueue&);
  void operator=(const BlockingQueue&);
};

}  // namespace port
}  // namespace voyager

#endif  // VOYAGER_PORT_BLOCKINGQUEUE_H_
