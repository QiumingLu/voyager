#ifndef VOYAGER_PORT_COUNTDOWNLATCH_H_
#define VOYAGER_PORT_COUNTDOWNLATCH_H_

#include "voyager/port/mutex.h"
#include "voyager/port/mutexlock.h"

namespace voyager {
namespace port {

class CountDownLatch {
 public:
  explicit CountDownLatch(int count)
    : count_(count),
      mutex_(),
      cond_(&mutex_)
  { }

  int GetCount() const {
    MutexLock lock(&mutex_);
    return count_;
  }

  void CountDown() {
    MutexLock lock(&mutex_);
    --count_;
    if (count_ == 0) {
      cond_.Signal();
    }
  }

  void Wait() {
    MutexLock lock(&mutex_);
    while (count_ > 0) {
      cond_.Wait();
    }
  }

 private:
  int count_;
  mutable Mutex mutex_;
  Condition cond_;
};

}  // namespace port
}  // namespace voyager

#endif  // VOYAGER_PORT_COUNTDOWNLATCH_H_
