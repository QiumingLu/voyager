// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/core/eventloop.h"

#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <algorithm>
#include <utility>

#include "voyager/core/dispatch.h"
#include "voyager/core/event_poll.h"
#include "voyager/core/event_select.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/core/timerlist.h"
#include "voyager/util/logging.h"
#include "voyager/util/timeops.h"

#ifdef __linux__
#include "voyager/core/event_epoll.h"
#else
#include "voyager/core/event_kqueue.h"
#endif

namespace voyager {
namespace {

thread_local EventLoop* runloop = nullptr;

class IgnoreSIGPIPE {
 public:
  IgnoreSIGPIPE() { ::signal(SIGPIPE, SIG_IGN); }
};

IgnoreSIGPIPE ignore;

}  // anonymous namespace

static EventPoller* CreatePoller(PollType type, EventLoop* loop) {
  EventPoller* poller = nullptr;
  switch (type) {
    case kSelect:
      poller = new EventSelect(loop);
      break;
    case kPoll:
      poller = new EventPoll(loop);
      break;
    case kEpoll:
#ifdef __linux__
      poller = new EventEpoll(loop);
#else
      poller = new EventKqueue(loop);
#endif
      break;
    default:
      VOYAGER_LOG(FATAL) << "error poll type.";
      assert(false);
      break;
  }
  return poller;
}

EventLoop* EventLoop::RunLoop() { return runloop; }

EventLoop::EventLoop(PollType type)
    : tid_(std::this_thread::get_id()),
      type_(type),
      exit_(true),
      run_(false),
      connection_size_(0),
      poller_(CreatePoller(type, this)),
      timers_(new TimerList(this)) {
  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, wakeup_fd_) == -1) {
    VOYAGER_LOG(FATAL) << "socketpair failed";
  }
  wakeup_dispatch_.reset(new Dispatch(this, wakeup_fd_[0]));

  VOYAGER_LOG(DEBUG) << "EventLoop " << this << " created in thread " << tid_;
  if (runloop) {
    VOYAGER_LOG(FATAL) << "Another EventLoop " << runloop
                       << " exists in this thread " << tid_;
  } else {
    runloop = this;
  }

  wakeup_dispatch_->SetReadCallback(std::bind(&EventLoop::HandleRead, this));
  wakeup_dispatch_->EnableRead();
}

EventLoop::~EventLoop() {
  runloop = nullptr;
  VOYAGER_LOG(DEBUG) << "EventLoop " << this << " of thread " << tid_
                     << " destructs in thread " << std::this_thread::get_id();

  wakeup_dispatch_->DisableAll();
  wakeup_dispatch_->RemoveEvents();
  ::close(wakeup_fd_[0]);
  ::close(wakeup_fd_[1]);
}

void EventLoop::Loop() {
  AssertInMyLoop();
  exit_ = false;
  std::vector<Dispatch*> dispatches;

  while (!exit_) {
    dispatches.clear();
    static const uint64_t kPollTimeMs = 5000;
    uint64_t t = (timers_->TimeoutMicros() / 1000);
    int timeout = static_cast<int>(std::min(t, kPollTimeMs));
    poller_->Poll(timeout, &dispatches);
    timers_->RunTimerProcs();

    for (std::vector<Dispatch*>::iterator it = dispatches.begin();
         it != dispatches.end(); ++it) {
      (*it)->HandleEvent();
    }
    RunFuncs();
  }
}

void EventLoop::Exit() {
  exit_ = true;
  if (!IsInMyLoop()) {
    WakeUp();
  }
}

void EventLoop::RunInLoop(const Func& func) {
  if (IsInMyLoop()) {
    func();
  } else {
    QueueInLoop(func);
  }
}

void EventLoop::QueueInLoop(const Func& func) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    funcs_.push_back(func);
  }

  // "必要时"有两种情况：
  // 1、如果调用QueueInLoop()的线程不是IO线程，那么唤醒是必需的；
  // 2、如果在IO线程调用QueueInLoop(),而此时正在调用RunFuncs
  if (!IsInMyLoop() || run_) {
    WakeUp();
  }
}

void EventLoop::RunInLoop(Func&& func) {
  if (IsInMyLoop()) {
    func();
  } else {
    QueueInLoop(std::move(func));
  }
}

void EventLoop::QueueInLoop(Func&& func) {
  {
    std::lock_guard<std::mutex> lock(mutex_);
    funcs_.push_back(std::move(func));
  }
  // "必要时"有两种情况：
  // 1、如果调用QueueInLoop()的线程不是IO线程，那么唤醒是必需的；
  // 2、如果在IO线程调用QueueInLoop(),而此时正在调用RunFuncQueue
  if (!IsInMyLoop() || run_) {
    WakeUp();
  }
}

TimerId EventLoop::RunAt(uint64_t micros_value, const TimerProcCallback& cb) {
  return timers_->Insert(micros_value, 0, cb);
}

TimerId EventLoop::RunAfter(uint64_t micros_delay,
                            const TimerProcCallback& cb) {
  uint64_t micros_value = timeops::NowMicros() + micros_delay;
  return timers_->Insert(micros_value, 0, cb);
}

TimerId EventLoop::RunEvery(uint64_t micros_interval,
                            const TimerProcCallback& cb) {
  uint64_t micros_value = timeops::NowMicros() + micros_interval;
  return timers_->Insert(micros_value, micros_interval, cb);
}

TimerId EventLoop::RunAt(uint64_t micros_value, TimerProcCallback&& cb) {
  return timers_->Insert(micros_value, 0, std::move(cb));
}

TimerId EventLoop::RunAfter(uint64_t micros_delay, TimerProcCallback&& cb) {
  uint64_t micros_value = timeops::NowMicros() + micros_delay;
  return timers_->Insert(micros_value, 0, std::move(cb));
}

TimerId EventLoop::RunEvery(uint64_t micros_interval, TimerProcCallback&& cb) {
  uint64_t micros_value = timeops::NowMicros() + micros_interval;
  return timers_->Insert(micros_value, micros_interval, std::move(cb));
}

void EventLoop::RemoveTimer(TimerId t) { timers_->Erase(t); }

void EventLoop::RemoveDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  AssertInMyLoop();
  poller_->RemoveDispatch(dispatch);
}

void EventLoop::UpdateDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  AssertInMyLoop();
  poller_->UpdateDispatch(dispatch);
}

bool EventLoop::HasDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  AssertInMyLoop();
  return poller_->HasDispatch(dispatch);
}

void EventLoop::AddConnection(const TcpConnectionPtr& ptr) {
  assert(ptr->OwnerEventLoop() == this);
  AssertInMyLoop();
  assert(connections_.find(ptr->name()) == connections_.end());
  connections_[ptr->name()] = ptr;
  ++connection_size_;
}

void EventLoop::RemoveConnection(const TcpConnectionPtr& ptr) {
  assert(ptr->OwnerEventLoop() == this);
  AssertInMyLoop();
  assert(connections_.find(ptr->name()) != connections_.end());
  connections_.erase(ptr->name());
  --connection_size_;
}

void EventLoop::RunFuncs() {
  std::vector<Func> funcs;
  run_ = true;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    funcs.swap(funcs_);
  }
  for (std::vector<Func>::iterator it = funcs.begin(); it != funcs.end();
       ++it) {
    (*it)();
  }
  run_ = false;
}

void EventLoop::WakeUp() {
  uint64_t one = 0;
  ssize_t n = ::write(wakeup_fd_[1], &one, sizeof(one));
  if (n != sizeof(one)) {
    VOYAGER_LOG(ERROR) << "EventLoop::WakeUp -  writes " << n
                       << " bytes instead of 8";
  }
}

void EventLoop::HandleRead() {
  uint64_t one = 0;
  ssize_t n = ::read(wakeup_fd_[0], &one, sizeof(one));
  if (n != sizeof(one)) {
    VOYAGER_LOG(ERROR) << "EventLoop::HandleRead - reads " << n
                       << " bytes instead of 8";
  }
}

void EventLoop::Abort() {
  VOYAGER_LOG(FATAL) << "EventLoop::Abort - (EventLoop " << this
                     << ") was created in tid_ = " << tid_
                     << ", current thread's tid_ = "
                     << std::this_thread::get_id();
}

}  // namespace voyager
