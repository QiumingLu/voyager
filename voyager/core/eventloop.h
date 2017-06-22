// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_EVENTLOOP_H_
#define VOYAGER_CORE_EVENTLOOP_H_

#include <unistd.h>

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "voyager/core/callback.h"
#include "voyager/port/currentthread.h"
#include "voyager/port/mutexlock.h"

namespace voyager {

class Dispatch;
class EventPoller;
class Timer;
class TimerList;

typedef std::pair<uint64_t, Timer*> TimerId;

enum PollType { kSelect, kPoll, kEpoll };

class EventLoop {
 public:
  typedef std::function<void()> Func;

  explicit EventLoop(PollType type = kEpoll);
  ~EventLoop();

  void Loop();

  void RunInLoop(const Func& func);
  void QueueInLoop(const Func& func);

  void RunInLoop(Func&& func);
  void QueueInLoop(Func&& func);

  TimerId RunAt(uint64_t micros_value, const TimerProcCallback& cb);
  TimerId RunAfter(uint64_t micros_delay, const TimerProcCallback& cb);
  TimerId RunEvery(uint64_t micros_interval, const TimerProcCallback& cb);

  TimerId RunAt(uint64_t micros_value, TimerProcCallback&& cb);
  TimerId RunAfter(uint64_t micros_delay, TimerProcCallback&& cb);
  TimerId RunEvery(uint64_t micros_interval, TimerProcCallback&& cb);

  void RemoveTimer(TimerId t);

  void AssertInMyLoop() {
    if (!IsInMyLoop()) {
      Abort();
    }
  }

  bool IsInMyLoop() const { return tid_ == port::CurrentThread::Tid(); }
  PollType GetPollType() const { return type_; }

  void Exit();

  // the eventloop of current thread.
  static EventLoop* RunLoop();

  // only internal use
  void RemoveDispatch(Dispatch* dispatch);
  void UpdateDispatch(Dispatch* dispatch);
  bool HasDispatch(Dispatch* dispatch);

  void AddConnection(const TcpConnectionPtr& ptr);
  void RemoveConnection(const TcpConnectionPtr& ptr);
  int ConnectionSize() const { return connection_size_; }

  static int AllConnectionSize() { return all_connection_size_; }

 private:
  void RunFuncs();
  void HandleRead();
  void Abort();
  void WakeUp();

  static std::atomic<int> all_connection_size_;

  PollType type_;

  bool exit_;
  bool run_;

  const uint64_t tid_;
  std::atomic<int> connection_size_;
  std::unique_ptr<EventPoller> poller_;
  std::unique_ptr<TimerList> timers_;

  int wakeup_fd_[2];
  std::unique_ptr<Dispatch> wakeup_dispatch_;

  port::Mutex mu_;
  std::vector<Func> funcs_;
  std::unordered_map<std::string, TcpConnectionPtr> connections_;

  // No copying allowed
  EventLoop(const EventLoop&);
  void operator=(const EventLoop&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_EVENTLOOP_H_
