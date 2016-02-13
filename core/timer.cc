#include "core/timer.h"

#include <stdint.h>
#include <unistd.h>
#include <sys/timerfd.h>
#include "core/eventloop.h"
#include "core/socket_util.h"
#include "util/logging.h"
#include "util/stl_util.h"

namespace mirants {
namespace timeops{
  
int CreateTimerfd() {
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd == -1) {
    MIRANTS_LOG(FATAL) << "timerfd_create: " << strerror(errno);
  }
  return timerfd;
}

struct timespec FromNow(Timestamp t) {
  int64_t microseconds = t.MicroSecondsSinceEpoch() 
                         - Timestamp::Now().MicroSecondsSinceEpoch();
  if (microseconds < 100) {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds/kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % kMicroSecondsPerSecond) * 1000);
  return ts;
}

void SetTimerfd(int fd, Timestamp t) {
  struct itimerspec new_value;
  struct itimerspec old_value;
  memset(&new_value, 0, sizeof(new_value));
  memset(&old_value, 0, sizeof(old_value));
  new_value.it_value = FromNow(t);
  if (::timerfd_settime(fd, 0, &new_value, &old_value) == -1) {
    MIRANTS_LOG(ERROR) << "timerfd_settime: " << strerror(errno);
  }
}

}  // namespace timers

TimerEvent::TimerEvent(EventLoop* ev)
    : eventloop_(ev),
      timerfd_(timeops::CreateTimerfd()),
      dispatch_(ev, timerfd_) {
  dispatch_.SetReadCallback(std::bind(&TimerEvent::HandleRead, this));
  dispatch_.EnableRead();
}

TimerEvent::~TimerEvent() {
  dispatch_.DisableAll();
  dispatch_.RemoveEvents();
  sockets::CloseFd(timerfd_);
}

int64_t TimerEvent::AddTimer(const TimeProcCallback& func,
                             Timestamp t,
                             double interval) {
  int64_t id = seq_.GetNext();
  Timer* timer = new Timer(func, t, interval, id);
  eventloop_->RunInLoop(std::bind(&TimerEvent::AddTimerInLoop, this, timer));
  return id;
}

int64_t TimerEvent::AddTimer(TimeProcCallback&& func,
                             Timestamp t,
                             double interval) {
  int64_t id = seq_.GetNext();
  Timer* timer = new Timer(func, t, interval, id);
  eventloop_->RunInLoop(std::bind(&TimerEvent::AddTimerInLoop, this, timer));
  return id;
}

void TimerEvent::DeleteTimer(int64_t id) {
  eventloop_->RunInLoop(
      std::bind(&TimerEvent::DeleteTimerInLoop, this, id));
}

void TimerEvent::AddTimerInLoop(Timer* timer) {
  timeops::SetTimerfd(timerfd_, timer->time);
}

void TimerEvent::DeleteTimerInLoop(int64_t id) {
}

void TimerEvent::HandleRead() {
  eventloop_->AssertThreadSafe();
  uint64_t exp;
  ssize_t s = ::read(timerfd_, &exp, sizeof(exp));
  if (s != sizeof(uint64_t)) {
    MIRANTS_LOG(ERROR) << "read: " << strerror(errno);
  }

  std::vector<Timer*> res(GetExpired(Timestamp::Now()));

  calling_ = true;

  for (std::vector<Timer*>::iterator it = res.begin(); it != res.end(); ++it) {
    (*it)->timeproc();
  }
  calling_ = false;
}

std::vector<Timer*> TimerEvent::GetExpired(Timestamp now) {
  std::vector<Timer*> res;
  Entry entry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  std::set<Entry>::iterator end = timers_.lower_bound(entry);
  for (std::set<Entry>::iterator it = timers_.begin(); it != end; ++it) {
    res.push_back(it->second);
  }
  timers_.erase(timers_.begin(), end);
  return res;
}

void TimerEvent::Next(const std::vector<Timer*>& expired, Timestamp now) {
  Timestamp next;
  for (std::vector<Timer*>::const_iterator it = expired.begin();
       it != expired.end(); ++it) {
    if ((*it)->interval > 0.0 
        && cancel_timers_.find(*it) == cancel_timers_.end()) {
      (*it)->time = AddTime(now, (*it)->interval);
      Add(*it);
    } else {
      delete *it;
    }
  }
  if (!timers_.empty()) {
    next = timers_.begin()->second->time;
  }
  if (next.Valid()) {
    timeops::SetTimerfd(timerfd_, next);
  }
}

bool TimerEvent::Add(Timer* timer) {
  eventloop_->AssertThreadSafe();
  timers_.insert(Entry(timer->time, timer));
  return true;
}

}  // namespace mirants
