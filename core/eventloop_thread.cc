#include "core/eventloop_thread.h"
#include "core/eventloop.h"
#include "port/mutexlock.h"

namespace mirants {

EventLoopThread::EventLoopThread(const std::string& name)
    : eventloop_(NULL),
      mu_(),
      cond_(&mu_),
      thread_(std::bind(&EventLoopThread::ThreadFunc, this), name) {
}

EventLoopThread::~EventLoopThread() {
  if (eventloop_ != NULL) {
    thread_.Join();
  }
}

EventLoop* EventLoopThread::StartLoop() {
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

void EventLoopThread::ThreadFunc() {
  EventLoop ev;
  { 
    port::MutexLock l(&mu_);
    eventloop_ = &ev;
    cond_.Signal();
  }
  eventloop_->Loop();
  eventloop_ = NULL;
}

}  // namespace mirants
