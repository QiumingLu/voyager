#include "voyager/core/eventloop.h"

#include <signal.h>
#ifdef __linux__
#include <sys/eventfd.h>
#endif
#include <unistd.h>

#include "voyager/core/dispatch.h"
#ifdef __linux__
#include "voyager/core/event_epoll.h"
#elif __APPLE__
#include "voyager/core/event_poll.h"
#endif
#include "voyager/core/socket_util.h"
#include "voyager/core/timer.h"
#include "voyager/util/logging.h"
#include "voyager/util/timestamp.h"

namespace voyager {
namespace {

__thread EventLoop* t_eventloop = NULL;

#ifdef __linux__
int CreateEventfd() {
  int fd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (fd < 0) {
    VOYAGER_LOG(ERROR) << "Failed in eventfd";
    abort();
  }
  return fd;
}
#endif

class IgnoreSIGPIPE {
 public:
  IgnoreSIGPIPE() {
    ::signal(SIGPIPE, SIG_IGN);
  }
};

IgnoreSIGPIPE ignore_SIGPIPE;

}  // namespace anonymous

EventLoop* EventLoop::EventLoopOfCurrentThread() {
  return t_eventloop;
}

#ifdef __linux__
EventLoop::EventLoop()
    : exit_(false),
      runfuncqueue_(false),
      tid_(port::CurrentThread::Tid()),
      poller_(new EventEpoll(this)),
      timer_ev_(new TimerEvent(this)),
      wakeup_fd_(CreateEventfd()),
      wakeup_dispatch_(new Dispatch(this, wakeup_fd_)) {
        
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
      runfuncqueue_(false),
      tid_(port::CurrentThread::Tid()),
      poller_(new EventPoll(this)),
      timer_ev_(new TimerEvent(this)) {

  if (::socketpair(AF_UNIX, SOCK_STREAM, 0, wakeup_fd_) < 0) {
    VOYAGER_LOG(FATAL) << "socketpair failed";
  }
  wakeup_dispatch_.reset(new Dispatch(this, wakeup_fd_[0]));

  VOYAGER_LOG(DEBUG) << "EventLoop "<< this 
	                 << " created in thread " << tid_;
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
                     << " destructs in thread " 
					 << port::CurrentThread::Tid();

  wakeup_dispatch_->DisableAll();
  wakeup_dispatch_->RemoveEvents();
#ifdef __linux__
  ::close(wakeup_fd_);
#elif __APPLE__
  ::close(wakeup_fd_[0]);
  ::close(wakeup_fd_[1]);
#endif
  t_eventloop = NULL;  
}

void EventLoop::Loop() {
  AssertThreadSafe();
  exit_ = false;

  while(!exit_) {
    static const int kPollTime = 10*1000;
    std::vector<Dispatch*> dispatches;

#ifdef __linux__
    poller_->Poll(kPollTime, &dispatches);
#elif __APPLE__
    int t = timer_ev_->GetTimeout();
    int temp = std::min(t, kPollTime);
    poller_->Poll(temp, &dispatches);
    timer_ev_->HandleTimers();
#endif

    for (std::vector<Dispatch*>::iterator it = dispatches.begin();
        it != dispatches.end(); ++it) {
      (*it)->HandleEvent();
    }
    RunFuncQueue();
  }
}

void EventLoop::Exit() {
  exit_ = true;
  
  // 在必要时唤醒IO线程，让它及时终止循环。
  if (!IsInCreatedThread()) {
    WakeUp();
  }
}

void EventLoop::RunInLoop(const Func& func) {
  if (IsInCreatedThread()) {
    func();
  } else {
    QueueInLoop(func);
  }
}

void EventLoop::RunInLoop(Func&& func) {
  if (IsInCreatedThread()) {
    func();
  } else {
    QueueInLoop(std::move(func));
  }
}

void EventLoop::QueueInLoop(const Func& func) {
  {
    port::MutexLock lock(&mu_);
    funcqueue_.push_back(func);
  }

  // "必要时"有两种情况：
  // 1、如果调用QueueInLoop()的线程不是IO线程，那么唤醒是必需的；
  // 2、如果在IO线程调用QueueInLoop(),而此时正在调用RunFuncQueue
  if (!IsInCreatedThread() || runfuncqueue_) {
    WakeUp();
  }
}

void EventLoop::QueueInLoop(Func&& func) {
  {
    port::MutexLock lock(&mu_);
    funcqueue_.push_back(std::move(func));
  }
  // "必要时"有两种情况：
  // 1、如果调用QueueInLoop()的线程不是IO线程，那么唤醒是必需的；
  // 2、如果在IO线程调用QueueInLoop(),而此时正在调用RunFuncQueue
  if (!IsInCreatedThread() || runfuncqueue_) {
    WakeUp();
  }
}

Timer* EventLoop::RunAt(const Timestamp& t, const TimeProcCallback& timeproc) {
  return timer_ev_->AddTimer(timeproc, t, 0.0);
}

Timer* EventLoop::RunAfter(double delay, const TimeProcCallback& timeproc) {
  Timestamp t(AddTime(Timestamp::Now(), delay));
  return RunAt(t, timeproc);
}

Timer* EventLoop::RunEvery(double interval, const TimeProcCallback& timeproc) {
  Timestamp t(AddTime(Timestamp::Now(), interval));
  return timer_ev_->AddTimer(timeproc, t, interval);
}

Timer* EventLoop::RunAt(const Timestamp& t, TimeProcCallback&& timeproc) {
  return timer_ev_->AddTimer(std::move(timeproc), t, 0.0);
}

Timer* EventLoop::RunAfter(double delay, TimeProcCallback&& timeproc) {
  Timestamp t(AddTime(Timestamp::Now(), delay));
  return RunAt(t, std::move(timeproc));
}

Timer* EventLoop::RunEvery(double interval, TimeProcCallback&& timeproc) {
  Timestamp t(AddTime(Timestamp::Now(), interval));
  return timer_ev_->AddTimer(timeproc, t, interval);
}

void EventLoop::DeleteTimer(Timer* t) {
  timer_ev_->DeleteTimer(t);
}

void EventLoop::RemoveDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  this->AssertThreadSafe();
  poller_->RemoveDispatch(dispatch);
}

void EventLoop::UpdateDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  this->AssertThreadSafe();
  poller_->UpdateDispatch(dispatch);
}

bool EventLoop::HasDispatch(Dispatch* dispatch) {
  assert(dispatch->OwnerEventLoop() == this);
  this->AssertThreadSafe();
  return poller_->HasDispatch(dispatch);
}

void EventLoop::RunFuncQueue() {
  std::vector<Func> funcs;
  runfuncqueue_ = true;
  {
    port::MutexLock lock(&mu_);
    funcs.swap(funcqueue_);
  }
  for (std::vector<Func>::iterator it = funcs.begin(); 
       it != funcs.end(); ++it) {
    (*it)();
  }
  runfuncqueue_ = false;
}

void EventLoop::WakeUp() {
  uint64_t one = 1;
#ifdef __linux__
  ssize_t n  = sockets::Write(wakeup_fd_, &one, sizeof(one));
#elif __APPLE__
    ssize_t n  = sockets::Write(wakeup_fd_[1], &one, sizeof(one));
#endif
  if (n != sizeof(one)) {
    VOYAGER_LOG(ERROR) << "EventLoop::WakeUp - " << wakeup_fd_ << " writes "
                       << n << " bytes instead of 8";
  }
}

void EventLoop::HandleRead() {
  uint64_t one = 1;
#ifdef __linux__
  ssize_t n = sockets::Read(wakeup_fd_, &one, sizeof(one));
#elif __APPLE__
    ssize_t n = sockets::Read(wakeup_fd_[0], &one, sizeof(one));
#endif
  if (n != sizeof(one)) {
    VOYAGER_LOG(ERROR) << "EventLoop::HandleRead - " << wakeup_fd_ << " reads "
                       << n << " bytes instead of 8";
  }
}

void EventLoop::AbortForNotInCreatedThread() {
  VOYAGER_LOG(FATAL) << "EventLoop::AbortForNotInCreatedThread - (EventLoop)"
                     << this << " was created in tid_ = " << tid_
                     << ", currentthread's tid_ = " 
                     << port::CurrentThread::Tid();
}

}  // namespace voyager
