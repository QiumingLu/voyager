#ifndef MIRANTS_PORT_MUTEX_H_
#define MIRANTS_PORT_MUTEX_H_

#include <pthread.h>

namespace mirants {
namespace port {

class Condition;
class Mutex {
 public:
  Mutex();
  ~Mutex();

  void Lock();
  void UnLock();
  void AssertHeld() { }

 private:
  friend class Condition;
  pthread_mutex_t mutex_;

  // No copying allow
  Mutex(const Mutex&);
  void operator=(const Mutex&);
};

class Condition {
 public:
  explicit Condition(Mutex* mutex);
  ~Condition();

  void Wait();
  void Signal();
  void SignalAll();

 private:
  pthread_cond_t cond_;
  Mutex* mutex_;
};

}  // namespace port
}  // namespace mirants

#endif  // MIRANTS_PORT_MUTEX_H_
