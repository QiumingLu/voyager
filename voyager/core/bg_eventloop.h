// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_BG_EVENTLOOP_H_
#define VOYAGER_CORE_BG_EVENTLOOP_H_

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>

#include "voyager/core/eventloop.h"

namespace voyager {

class BGEventLoop {
 public:
  explicit BGEventLoop(PollType type = kEpoll);
  ~BGEventLoop();

  EventLoop* Loop();

 private:
  void ThreadFunc();

  PollType type_;
  EventLoop* eventloop_;
  std::mutex mutex_;
  std::condition_variable cv_;
  std::unique_ptr<std::thread> thread_;

  // No copying allowed
  BGEventLoop(const BGEventLoop&);
  void operator=(const BGEventLoop&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_BG_EVENTLOOP_H_