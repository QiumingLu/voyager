#include "voyager/core/timer.h"

#include <stdint.h>
#include <unistd.h>

#ifdef __linux__
#include <sys/timerfd.h>
#endif

#include "voyager/core/eventloop.h"
#include "voyager/core/socket_util.h"
#include "voyager/util/logging.h"
#include "voyager/util/stl_util.h"

namespace voyager {

#ifdef __linux__  
namespace timeops{

int CreateTimerfd() {
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (timerfd == -1) {
    VOYAGER_LOG(FATAL) << "timerfd_create: " << strerror(errno);
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
      microseconds / kMicroSecondsPerSecond);
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
    VOYAGER_LOG(ERROR) << "timerfd_settime: " << strerror(errno);
  }
}

}  // namespace timeops

#elif __APPLE__
namespace timeops {
int FromNow(Timestamp t) {
  int64_t microseconds = t.MicroSecondsSinceEpoch() 
                         - Timestamp::Now().MicroSecondsSinceEpoch();
  if (microseconds < 1000) {
    microseconds = 1000;
  }
  return static_cast<int>(microseconds / 1000);
}

}  // namespace timeops
#endif

struct Timer {
  TimeProcCallback timeproc;
  Timestamp time;
  double interval;
  bool repeat;

  Timer(const TimeProcCallback& func, Timestamp t, double inter)
      : timeproc(func), time(t), interval(inter), repeat(false) {
    if (interval > 0.0) {
      repeat = true;
    }
  }

  Timer(TimeProcCallback&& func, Timestamp t, double inter)
      : timeproc(std::move(func)), 
        time(t), 
        interval(inter),
        repeat(false) {
    if (interval > 0.0) {
      repeat = true;
    }
  }
};

#ifdef __linux__
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
  close(timerfd_);
  STLDeleteValues(&timers_);
}

#elif __APPLE__
TimerEvent::TimerEvent(EventLoop* ev)
    : eventloop_(ev),
      calling_(false) {
}

TimerEvent::~TimerEvent() {
  STLDeleteValues(&timers_);
}
#endif

Timer* TimerEvent::AddTimer(const TimeProcCallback& func,
                             Timestamp t,
                             double interval) {
  Timer* timer = new Timer(func, t, interval);
  eventloop_->RunInLoop(std::bind(&TimerEvent::AddTimerInLoop, this, timer));
  return timer;
}

Timer* TimerEvent::AddTimer(TimeProcCallback&& func,
                             Timestamp t,
                             double interval) {
  Timer* timer = new Timer(std::move(func), t, interval);
  eventloop_->RunInLoop(std::bind(&TimerEvent::AddTimerInLoop, this, timer));
  return timer;
}

void TimerEvent::DeleteTimer(Timer* timer) {
  eventloop_->RunInLoop(
      std::bind(&TimerEvent::DeleteTimerInLoop, this, timer));
}

#ifdef __linux__
void TimerEvent::AddTimerInLoop(Timer* timer) {
  eventloop_->AssertInMyLoop();
  bool isreset = Add(timer);
  if (isreset) {
    timeops::SetTimerfd(timerfd_, timer->time);
  }
}
#elif __APPLE__
void TimerEvent::AddTimerInLoop(Timer* timer) {
  eventloop_->AssertInMyLoop();
  Add(timer);
}
#endif 

void TimerEvent::DeleteTimerInLoop(Timer* timer) {
  eventloop_->AssertInMyLoop();
  Entry entry(timer->time, timer);
  std::set<Entry>::iterator it = timers_.find(entry);
  if (it != timers_.end()) {
    delete it->second;
    timers_.erase(it);
  } else if (calling_) {
    delete_timers_.insert(timer);
  }
}

#ifdef __linux__
void TimerEvent::HandleRead() {
  eventloop_->AssertInMyLoop();
  uint64_t exp;
  ssize_t s = ::read(timerfd_, &exp, sizeof(exp));
  if (s != sizeof(uint64_t)) {
    VOYAGER_LOG(ERROR) << "read: " << strerror(errno);
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

#elif __APPLE__
void TimerEvent::HandleTimers() {
  eventloop_->AssertInMyLoop();
  Timestamp now = Timestamp::Now();
  std::vector<Timer*> res(GetExpired(now));

  calling_ = true;
  for (std::vector<Timer*>::iterator it = res.begin(); it != res.end(); ++it) {
    (*it)->timeproc();
  }
  calling_ = false;

  Next(res, now);
}

int TimerEvent::GetTimeout() const {
  eventloop_->AssertInMyLoop();
  if (timers_.empty()) {
    return 100000;
  } else {
    return timeops::FromNow(timers_.begin()->second->time);
  }
}
#endif

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
#ifdef __linux__
  if (next_time.Valid()) {
    timeops::SetTimerfd(timerfd_, next_time);
  }
#endif
}

bool TimerEvent::Add(Timer* timer) {
  eventloop_->AssertInMyLoop();
  
  bool isreset = false;
  Timestamp t = timer->time;
  if (timers_.empty() || t < timers_.begin()->first) {
    isreset = true;
  }

  timers_.insert(Entry(timer->time, timer));
  
  return isreset;
}

}  // namespace voyager
