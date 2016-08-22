#include "voyager/core/timerlist.h"
#include "voyager/core/eventloop.h"
#include "voyager/util/logging.h"
#include "voyager/util/stl_util.h"
#include "voyager/util/timeops.h"


namespace voyager {

struct TimerList::Timer {
  Timer(uint64_t value, uint64_t interval, const TimerProcCallback& cb)
      : micros_value(value),
        micros_interval(interval),
        timerproc_cb(cb),
        repeat(false) {
    if (micros_interval > 0) {
      repeat = true;
    }
  }

  Timer(uint64_t value, uint64_t interval, TimerProcCallback&& cb)
      : micros_value(value),
        micros_interval(interval),
        timerproc_cb(std::move(cb)),
        repeat(false) {
    if (micros_interval > 0) {
      repeat = true;
    }
  }

 private:
  friend class TimerList;

  uint64_t micros_value;
  uint64_t micros_interval;
  TimerProcCallback timerproc_cb;
  bool repeat;
};


TimerList::TimerList(EventLoop* ev)
    : eventloop_(CHECK_NOTNULL(ev)) {
}

TimerList::~TimerList() {
  STLDeleteValues(&timers_);
}

TimerList::Timer* TimerList::Insert(uint64_t micros_value,
                                    uint64_t micros_interval,
                                    const TimerProcCallback& cb) {
  Timer* timer = new Timer(micros_value, micros_interval, cb);
  eventloop_->RunInLoop([this, timer]() {
      this->InsertInLoop(timer);
  });
  return timer;
}

TimerList::Timer* TimerList::Insert(uint64_t micros_value,
                                    uint64_t micros_interval,
                                    TimerProcCallback&& cb) {
  Timer* timer = new Timer(micros_value, micros_interval, std::move(cb));
  eventloop_->RunInLoop([this, timer]() {
      this->InsertInLoop(timer);
  });
  return timer;
}

void TimerList::Erase(Timer* timer) {
  eventloop_->RunInLoop([this, timer]() {
      this->EraseInLoop(timer);
  });
}

void TimerList::InsertInLoop(Timer* timer) {
  eventloop_->AssertInMyLoop();
  timers_.insert(Entry(timer->micros_value, timer));
}

void TimerList::EraseInLoop(Timer* timer) {
  eventloop_->AssertInMyLoop();
  Entry entry(timer->micros_value, timer);
  std::set<Entry>::iterator it = timers_.find(entry);
  if (it != timers_.end()) {
    delete it->second;
    timers_.erase(it);
  }
}

uint64_t TimerList::TimeoutMicros() const {
  eventloop_->AssertInMyLoop();
  if (timers_.empty()) {
    return 10000;
  }
  return (timers_.begin()->first - timeops::NowMicros());
}

void TimerList::RunTimerProcs() {
  eventloop_->AssertInMyLoop();
  if (timers_.empty()) {
    return;
  }

  uint64_t micros_now = timeops::NowMicros();
  std::vector<Timer*> res(ExpiredTimers(micros_now));

  for (size_t i = 0; i < res.size(); ++i) {
    res[i]->timerproc_cb();

    if (res[i]->repeat) {
      res[i]->micros_value = micros_now + res[i]->micros_interval;
      timers_.insert(Entry(res[i]->micros_value, res[i]));
    } else {
      delete res[i];
    }
  }
}

std::vector<TimerList::Timer*> TimerList::ExpiredTimers(uint64_t micros) {
  std::vector<Timer*> res;
  Entry entry(micros, reinterpret_cast<Timer*>(UINTPTR_MAX));
  std::set<Entry>::iterator end = timers_.lower_bound(entry);
  for (std::set<Entry>::iterator it = timers_.begin(); it != end; ++it) {
    res.push_back(it->second);
  }
  timers_.erase(timers_.begin(), end);
  return res;
}

}  // namespace voyager
