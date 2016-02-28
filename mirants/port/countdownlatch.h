#ifndef MIRANTS_PORT_COUNTDOWNLATCH_H_
#define MIRANTS_PORT_COUNTDOWNLATCH_H_

#include "mirants/port/mutex.h"
#include "mirants/port/mutexlock.h"

namespace mirants {
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
}  // namespace mirants

#endif  // MIRANTS_PORT_COUNTDOWNLATCH_H_
