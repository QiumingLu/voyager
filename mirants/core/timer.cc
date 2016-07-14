#ifndef __MACH__
#include "mirants/core/timer.h"

#include <stdint.h>
#include <unistd.h>
#include <sys/timerfd.h>

#include "mirants/core/eventloop.h"
#include "mirants/core/socket_util.h"
#include "mirants/util/logging.h"
#include "mirants/util/stl_util.h"

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

port::SequenceNumber TimerEvent::seq_;

TimerEvent::TimerEvent(EventLoop* ev)
    : eventloop_(ev),
      timerfd_(timeops::CreateTimerfd()),
      dispatch_(ev, timerfd_),
      calling_(false) {
  dispatch_.SetReadCallback(std::bind(&TimerEvent::HandleRead, this));
  dispatch_.EnableRead();
}

TimerEvent::~TimerEvent() {
  dispatch_.DisableAll();
  dispatch_.RemoveEvents();
  sockets::CloseFd(timerfd_);
  STLDeleteValues(&timers_);
}

Timer* TimerEvent::AddTimer(const TimeProcCallback& func,
                             Timestamp t,
                             double interval) {
  int64_t id = seq_.GetNext();
  Timer* timer = new Timer(func, t, interval, id);
  eventloop_->RunInLoop(std::bind(&TimerEvent::AddTimerInLoop, this, timer));
  return timer;
}

Timer* TimerEvent::AddTimer(TimeProcCallback&& func,
                             Timestamp t,
                             double interval) {
  int64_t id = seq_.GetNext();
  Timer* timer = new Timer(std::move(func), t, interval, id);
  eventloop_->RunInLoop(std::bind(&TimerEvent::AddTimerInLoop, this, timer));
  return timer;
}

void TimerEvent::DeleteTimer(Timer* timer) {
  eventloop_->RunInLoop(
      std::bind(&TimerEvent::DeleteTimerInLoop, this, timer));
}

void TimerEvent::AddTimerInLoop(Timer* timer) {
  eventloop_->AssertThreadSafe();
  bool isreset = Add(timer);
  if (isreset) {
    timeops::SetTimerfd(timerfd_, timer->time);
  }
}

void TimerEvent::DeleteTimerInLoop(Timer* timer) {
  eventloop_->AssertThreadSafe();
  Entry entry(timer->time, timer);
  std::set<Entry>::iterator it = timers_.find(entry);
  if (it != timers_.end()) {
    delete it->second;
    timers_.erase(it);
  } else if (calling_) {
    delete_timers_.insert(timer);
  }
}

void TimerEvent::HandleRead() {
  eventloop_->AssertThreadSafe();
  uint64_t exp;
  ssize_t s = ::read(timerfd_, &exp, sizeof(exp));
  if (s != sizeof(uint64_t)) {
    MIRANTS_LOG(ERROR) << "read: " << strerror(errno);
  }

  Timestamp now = Timestamp::Now();
  std::vector<Timer*> res(GetExpired(now));

  calling_ = true;
  for (std::vector<Timer*>::iterator it = res.begin(); it != res.end(); ++it) {
    (*it)->timeproc();
  }
  calling_ = false;

  Next(res, now);
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
  Timestamp next_time;
  for (std::vector<Timer*>::const_iterator it = expired.begin();
       it != expired.end(); ++it) {
    if ((*it)->repeat
        && delete_timers_.find(*it) == delete_timers_.end()) {
      (*it)->time = AddTime(now, (*it)->interval);
      Add(*it);
    } else {
      delete *it;
    }
  }
  if (!timers_.empty()) {
    next_time = timers_.begin()->second->time;
  }
  if (next_time.Valid()) {
    timeops::SetTimerfd(timerfd_, next_time);
  }
}

bool TimerEvent::Add(Timer* timer) {
  eventloop_->AssertThreadSafe();
  
  bool isreset = false;
  Timestamp t = timer->time;
  if (timers_.empty() || t < timers_.begin()->first) {
    isreset = true;
  }

  timers_.insert(Entry(timer->time, timer));
  
  return isreset;
}

}  // namespace mirants

#endif
