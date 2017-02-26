// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_PORT_MUTEXLOCK_H_
#define VOYAGER_PORT_MUTEXLOCK_H_

#include "voyager/port/mutex.h"

namespace voyager {
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

  // No copying allowed
  MutexLock(const MutexLock&);
  void operator=(const MutexLock&);
};

}  // namespace port
}  // namespace voyager

#endif  // VOYAGER_PORT_MUTEXLOCK_H_
