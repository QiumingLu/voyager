#include "voyager/core/timerlist.h"
#include "voyager/core/eventloop.h"
#include "voyager/util/logging.h"
#include "voyager/util/stl_util.h"
#include "voyager/util/timeops.h"


namespace voyager {

class Timer {
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

TimerId TimerList::Insert(uint64_t micros_value,
                          uint64_t micros_interval,
                          const TimerProcCallback& cb) {
  TimerId id(micros_value, new Timer(micros_value, micros_interval, cb));
  eventloop_->RunInLoop([this, id]() {
    this->InsertInLoop(id);
  });
  return id;
}

TimerId TimerList::Insert(uint64_t micros_value,
                          uint64_t micros_interval,
                          TimerProcCallback&& cb) {
  TimerId id(micros_value,
             new Timer(micros_value, micros_interval, std::move(cb)));
  eventloop_->RunInLoop([this, id]() {
    this->InsertInLoop(id);
  });
  return id;
}

void TimerList::Erase(TimerId id) {
  eventloop_->RunInLoop([this, id]() {
    this->EraseInLoop(id);
  });
}

void TimerList::InsertInLoop(TimerId id) {
  eventloop_->AssertInMyLoop();
  timers_.insert(id);
}

void TimerList::EraseInLoop(TimerId id) {
  eventloop_->AssertInMyLoop();
  std::set<TimerId>::iterator it = timers_.find(id);
  if (it != timers_.end()) {
    delete it->second;
    timers_.erase(it);
  }
}

uint64_t TimerList::TimeoutMicros() const {
  eventloop_->AssertInMyLoop();
  if (timers_.empty()) {
    return 5000000;
  }
  if (timers_.begin()->first < timeops::NowMicros()) {
    return 0;
  } else {
    return (timers_.begin()->first - timeops::NowMicros());
  }
}

void TimerList::RunTimerProcs() {
  eventloop_->AssertInMyLoop();
  if (timers_.empty()) {
    return;
  }

  uint64_t micros_now = timeops::NowMicros();
  std::set<TimerId>::iterator it;
  while (true) {
    it = timers_.begin();
    if (it != timers_.end() && it->first <= micros_now) {
      uint64_t micros_value = it->first;
      Timer* timer = it->second;
      timers_.erase(it);
      timer->timerproc_cb();
      if (timer->repeat) {
        micros_value += timer->micros_interval;
        timers_.insert(TimerId(micros_value, timer));
      } else {
        delete timer;
      }
    } else {
      break;
    }
  }
}

}  // namespace voyager
