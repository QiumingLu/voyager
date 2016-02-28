#ifndef MIRANTS_PORT_BLOCKINGQUEUE_H_
#define MIRANTS_PORT_BLOCKINGQUEUE_H_

#include <deque>
#include <utility>
#include <assert.h>
#include "mirants/port/mutex.h"
#include "mirants/port/mutexlock.h"

namespace mirants {
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

  // No copying allow
  BlockingQueue(const BlockingQueue&);
  void operator=(const BlockingQueue&);
};

}  // namespace port
}  // namespace mirants

#endif  // MIRANTS_PORT_BLOCKINGQUEUE_H_
