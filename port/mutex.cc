#include "port/mutex.h"

#include <stdio.h>
#include <string.h>
#include "util/logging.h"

namespace mirants {
namespace port {

static void PthreadCall(const char* label, int result) {
  if (result != 0) {
    MIRANTS_LOG(FATAL) << label << strerror(result);
  }
}

Mutex::Mutex() { 
  PthreadCall("pthread_mutex_init: ",pthread_mutex_init(&mutex_, NULL)); 
}

Mutex::~Mutex() { 
  PthreadCall("pthread_mutex_destory: ", pthread_mutex_destory(&mutex_));
}

void Mutex::Lock() {
  PthreadCall("pthread_mutex_lock: ", pthread_mutex_lock(&mutex_));
}

void Mutex::UnLock() {
  PthreadCall("pthread_mutex_unlock: ", pthread_mutex_unlock(&mutex_));
}

Condition::Condition(Mutex* mutex) : mutex_(mutex) { 
  PthreadCall("pthread_cond_init: ", pthread_cond_init(&cond_, NULL));
}

Condition::~Condition() {
  PthreadCall("pthread_cond_destory: ", pthread_cond_destory(&cond_));
}

void Condition::Wait() {
  PthreadCall("pthread_cond_wait: ", pthread_cond_wait(&cond_, &mutex_->mutex_));
}

void Condition::Signal() {
  PthreadCall("pthread_cond_signal: ", pthread_cond_signal(&cond_));
}

void Condition::SignalAll() {
  PthreadCall("pthread_cond_broadcast: ", pthread_cond_broadcast(&cond_));
}

}  // namespace mirants 
}  // namespace port
