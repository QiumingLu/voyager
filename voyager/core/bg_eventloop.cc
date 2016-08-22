#include "voyager/core/bg_eventloop.h"

#include <assert.h>

#include "voyager/core/eventloop.h"
#include "voyager/port/mutexlock.h"
#include "voyager/util/logging.h"

namespace voyager {

BGEventLoop::BGEventLoop(const std::string& name)
    : eventloop_(NULL),
      mu_(),
      cond_(&mu_),
      thread_(std::bind(&BGEventLoop::ThreadFunc, this), name) {
}

BGEventLoop::~BGEventLoop() {
  if (eventloop_ != NULL) {
    eventloop_->Exit();
    thread_.Join();
  }
}

EventLoop* BGEventLoop::Loop() {
  assert(!thread_.Started());
  thread_.Start();
  {
    port::MutexLock l(&mu_);
    while (eventloop_ == NULL) {
      cond_.Wait();
    }
  }
  return eventloop_;
}

void BGEventLoop::ThreadFunc() {
  EventLoop ev;
  {
    port::MutexLock l(&mu_);
    eventloop_ = &ev;
    cond_.Signal();
  }
  eventloop_->Loop();
  eventloop_ = NULL;
}

}  // namespace voyager
