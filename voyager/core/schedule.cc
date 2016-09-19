#include "voyager/core/schedule.h"

#include "voyager/core/eventloop.h"
#include "voyager/core/online_connections.h"
#include "voyager/util/logging.h"

namespace voyager {

Schedule::Schedule(EventLoop* ev, int size)
    : baseloop_(CHECK_NOTNULL(ev)),
      size_(size),
      started_(false) {
}

Schedule::~Schedule() {
  port::Singleton<OnlineConnections>::Instance().Clear();
}

void Schedule::Start() {
  assert(!started_);
  baseloop_->AssertInMyLoop();
  started_ = true;
  OnlineConnections& instance
      = port::Singleton<OnlineConnections>::Instance();
  for (size_t i = 0; i < size_; ++i) {
    std::unique_ptr<BGEventLoop> loop(new BGEventLoop());
    instance.Insert(loop->Loop());
    loops_.push_back(std::move(loop));
  }
  if (size_ == 0) {
    instance.Insert(baseloop_);
  }
}

EventLoop* Schedule::AssignLoop() {
  baseloop_->AssertInMyLoop();
  assert(started_);
  return port::Singleton<OnlineConnections>::Instance().GetLoop();
}

}  // namespace voyager
