#ifndef MIRANTS_PORT_MUTEXLOCK_H_
#define MIRANTS_PORT_MUTEXLOCK_H_

#include "mirants/port/mutex.h"

namespace mirants {
namespace port {

class MutexLock {
 public:
  explicit MutexLock(Mutex* mutex) 
      : mutex_(mutex) {
    mutex_->Lock();
  }

  ~MutexLock() { mutex_->UnLock(); }

 private:
  Mutex* const mutex_;

  // No copying allow
  MutexLock(const MutexLock&);
  void operator=(const MutexLock&);
};

}  // namespace port
}  // namespace mirants

#endif  // MIRANTS_PORT_MUTEXLOCK_H_
