#include "voyager/core/newtimer.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include <map>
#include <vector>
#include <utility>

#include "voyager/core/eventloop.h"
#include "voyager/port/singleton.h"
#include "voyager/util/logging.h"
#include "voyager/util/timeops.h"

namespace voyager {

NewTimer::NewTimer(EventLoop* ev, const TimerProcCallback& cb)
    : timerfd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
      repeat_(false),
      set_time_(false),
      eventloop_(CHECK_NOTNULL(ev)),
      dispatch_(ev, timerfd_),
      timerproc_cb_(cb) {
  if (timerfd_ == -1) {
    VOYAGER_LOG(FATAL) << "timerfd_create: " << strerror(errno);
  } else {
  	dispatch_.SetReadCallback(std::bind(&NewTimer::HandleRead, this));
    dispatch_.EnableRead();
  }
}

NewTimer::NewTimer(EventLoop* ev, TimerProcCallback&& cb)
    : timerfd_(::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC)),
      repeat_(false),
      set_time_(false),
      eventloop_(CHECK_NOTNULL(ev)),
      dispatch_(ev, timerfd_),
      timerproc_cb_(std::move(cb)) {
  if (timerfd_ == -1) {
    VOYAGER_LOG(FATAL) << "timerfd_create: " << strerror(errno);
  } else {
    dispatch_.SetReadCallback(std::bind(&NewTimer::HandleRead, this));
    dispatch_.EnableRead();
  }
}

NewTimer::~NewTimer() {
  dispatch_.DisableAll();
  dispatch_.RemoveEvents();
  if (::close(timerfd_) == -1) {
  	VOYAGER_LOG(ERROR) << "close: " << strerror(errno);
  }
}


void NewTimer::SetTime(uint64_t nanos_value, uint64_t nanos_interval) {
  eventloop_->RunInLoop([this, nanos_value, nanos_interval]() {
      this->SetTimeInLoop(nanos_value, nanos_interval);
  });
}

void NewTimer::SetTimeInLoop(uint64_t nanos_value, uint64_t nanos_interval) {
  eventloop_->AssertInMyLoop();
  struct itimerspec new_value;
  struct itimerspec old_value;
  memset(&new_value, 0, sizeof(new_value));
  memset(&old_value, 0, sizeof(old_value));

  new_value.it_interval.tv_sec = 
      static_cast<time_t>(nanos_interval /timeops::kNonasSecondsPerSecond);
  new_value.it_interval.tv_nsec = 
      static_cast<long>(nanos_interval % timeops::kNonasSecondsPerSecond);
  new_value.it_value.tv_sec = 
      static_cast<time_t>(nanos_value / timeops::kNonasSecondsPerSecond);
  new_value.it_value.tv_nsec = 
      static_cast<long>(nanos_value % timeops::kNonasSecondsPerSecond);

  if (::timerfd_settime(timerfd_, 0, &new_value, &old_value) == -1) {
    VOYAGER_LOG(ERROR) << "timerfd_settime: " << strerror(errno);
  }

  if (nanos_interval != 0) {
    repeat_ = true;
  }
}

void NewTimer::HandleRead() {
  eventloop_->AssertInMyLoop();
  uint64_t exp = 0;
  ssize_t s = ::read(timerfd_, &exp, sizeof(exp));
  if (s != sizeof(uint64_t)) {
    VOYAGER_LOG(ERROR) << "read: " << strerror(errno);
  }

  if (timerproc_cb_) {
    timerproc_cb_();
  }
}

}  // namespace timeops
