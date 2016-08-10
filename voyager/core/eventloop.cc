#include "voyager/core/eventloop.h"

#include <signal.h>
#include <unistd.h>

#include "voyager/core/dispatch.h"
#include "voyager/core/event_poll.h"
#include "voyager/core/online_connections.h"
#include "voyager/util/logging.h"
#include "voyager/util/timeops.h"

#ifdef __linux__
#include <sys/eventfd.h>
#include "voyager/core/event_epoll.h"
#endif

namespace voyager {
namespace {

__thread EventLoop* t_eventloop = NULL;

class IgnoreSIGPIPE {
 public:
  IgnoreSIGPIPE() {
    ::signal(SIGPIPE, SIG_IGN);
  }
};

IgnoreSIGPIPE ignore;

}  // namespace anonymous

EventLoop* EventLoop::EventLoopOfCurrentThread() {
  return t_eventloop;
}

#ifdef __linux__
EventLoop::EventLoop()
    : exit_(false),
      run_(false),
      tid_(port::CurrentThread::Tid()),
      poller_(new EventEpoll(this)),
      timers_(new TimerList(this)),
      wakeup_fd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC)),
      wakeup_dispatch_(new Dispatch(this, wakeup_fd_)) {
  if (fd == -1) {
    VOYAGER_LOG(FATAL) << "eventfd: " << strerror(errno);
  }      

  VOYAGER_LOG(INFO) << "EventLoop "<< this << " created in thread " << tid_;
  if (t_eventloop) {
    VOYAGER_LOG(FATAL) << "Another EventLoop " << t_eventloop
                       << " exists in this thread " << tid_;
  } else {
    t_eventloop = this;
  }

  wakeup_dispatch_->SetReadCallback(std::bind(&EventLoop::HandleRead, this));
  wakeup_dispatch_->EnableRead();
}
#elif __APPLE__
EventLoop::EventLoop()
    : exit_(false),
      run_(false),
      tid_(port::CurrentThread::Tid()),
      poller_(new EventPoll(this)),
      timers_(new TimerList(this)) {

  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, wakeup_fd_) == -1) {
    VOYAGER_LOG(FATAL) << "socketpair failed";
  }
  wakeup_dispatch_.reset(new Dispatch(this, wakeup_fd_[0]));

  VOYAGER_LOG(DEBUG) << "EventLoop "<< this << " created in thread " << tid_;
  if (t_eventloop) {
    VOYAGER_LOG(FATAL) << "Another EventLoop " << t_eventloop
                       << " exists in this thread " << tid_;
  } else {
    t_eventloop = this;
  }

  wakeup_dispatch_->SetReadCallback(std::bind(&EventLoop::HandleRead, this));
  wakeup_dispatch_->EnableRead();
}
#endif

EventLoop::~EventLoop() {
  t_eventloop = NULL;
  VOYAGER_LOG(DEBUG) << "EventLoop " << this << " of thread " << tid_
                     << " destructs in thread " << port::CurrentThread::Tid();

  port::Singleton<OnlineConnections>::Instance().Erase(this);
  wakeup_dispatch_->DisableAll();
  wakeup_dispatch_->RemoveEvents();
#ifdef __linux__
  ::close(wakeup_fd_);
#elif __APPLE__
  ::close(wakeup_fd_[0]);
  ::close(wakeup_fd_[1]);
#endif
}

void EventLoop::Loop() {
  AssertInMyLoop();
  exit_ = false;

  while(!exit_) {
    static const uint64_t kPollTimeMs = 10000;
    std::vector<Dispatch*> dispatches;
    uint64_t t = timers_->TimeoutMicros();
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
  this->QueueInLoop(std::bind(&EventLoop::ExitInLoop, this));
}

void EventLoop::ExitInLoop() {
  this->AssertInMyLoop();
  exit_ = true;
  // 在必要时唤醒IO线程，让它及时终止循环。
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
    port::MutexLock lock(&mu_);
    funcs_.push_back(func);
  }

  // "必要时"有两种情况：
  // 1、如果调用QueueInLoop()的线程不是IO线程，那么唤醒是必需的；
  // 2、如果在IO线程调用QueueInLoop(),而此时正在调用RunFuncQueue
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
    port::MutexLock lock(&mu_);
    funcs_.push_back(std::move(func));
  }
  // "必要时"有两种情况：
  // 1、如果调用QueueInLoop()的线程不是IO线程，那么唤醒是必需的；
  // 2、如果在IO线程调用QueueInLoop(),而此时正在调用RunFuncQueue
  if (!IsInMyLoop() || run_) {
    WakeUp();
  }
}

TimerList::Timer* EventLoop::RunAt(const TimerProcCallback& cb, uint64_t micros_value) {
  return timers_->Insert(cb, micros_value, 0);
}

TimerList::Timer* EventLoop::RunAfter(const TimerProcCallback& cb, uint64_t micros_delay) {
  uint64_t micros_value = timeops::NowMicros() + micros_delay;
  return timers_->Insert(cb, micros_value, 0);
}

TimerList::Timer* EventLoop::RunEvery(const TimerProcCallback& cb, uint64_t micros_interval) {
  uint64_t micros_value = timeops::NowMicros() + micros_interval;
  return timers_->Insert(cb, micros_value, micros_interval);
}

TimerList::Timer* EventLoop::RunAt(TimerProcCallback&& cb, uint64_t micros_value) {
  return timers_->Insert(std::move(cb), micros_value, 0);
}

TimerList::Timer* EventLoop::RunAfter(TimerProcCallback&& cb, uint64_t micros_delay) {
  uint64_t micros_value = timeops::NowMicros() + micros_delay;
  return timers_->Insert(std::move(cb), micros_value, 0);
}

TimerList::Timer* EventLoop::RunEvery(TimerProcCallback&& cb, uint64_t micros_interval) {
  uint64_t micros_value = timeops::NowMicros() + micros_interval;
  return timers_->Insert(cb, micros_value, micros_interval);
}

void EventLoop::RemoveTimer(TimerList::Timer* t) {
  timers_->Erase(t);
}

void EventLoop::RemoveDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  this->AssertInMyLoop();
  poller_->RemoveDispatch(dispatch);
}

void EventLoop::UpdateDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  this->AssertInMyLoop();
  poller_->UpdateDispatch(dispatch);
}

bool EventLoop::HasDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  this->AssertInMyLoop();
  return poller_->HasDispatch(dispatch);
}

void EventLoop::RunFuncs() {
  std::vector<Func> funcs;
  run_ = true;
  {
    port::MutexLock lock(&mu_);
    funcs.swap(funcs_);
  }
  for (std::vector<Func>::iterator it = funcs.begin(); 
       it != funcs.end(); ++it) {
    (*it)();
  }
  run_ = false;
}

void EventLoop::WakeUp() {
  uint64_t one = 0;
#ifdef __linux__
  ssize_t n  = ::write(wakeup_fd_, &one, sizeof(one));
#elif __APPLE__
  ssize_t n  = ::write(wakeup_fd_[1], &one, sizeof(one));
#endif
  if (n != sizeof(one)) {
    VOYAGER_LOG(ERROR) << "EventLoop::WakeUp - " << wakeup_fd_ << " writes "
                       << n << " bytes instead of 8";
  }
}

void EventLoop::HandleRead() {
  uint64_t one = 0;
#ifdef __linux__
  ssize_t n = ::read(wakeup_fd_, &one, sizeof(one));
#elif __APPLE__
  ssize_t n = ::read(wakeup_fd_[0], &one, sizeof(one));
#endif
  if (n != sizeof(one)) {
    VOYAGER_LOG(ERROR) << "EventLoop::HandleRead - " << wakeup_fd_ << " reads "
                       << n << " bytes instead of 8";
  }
}

void EventLoop::Abort() {
  VOYAGER_LOG(FATAL) << "EventLoop::Abort - (EventLoop " << this 
                     << ") was created in tid_ = " << tid_ 
                     << ", currentthread's tid_ = " 
                     << port::CurrentThread::Tid();
}

}  // namespace voyager
