// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_CORE_CALLBACK_H_
#define VOYAGER_CORE_CALLBACK_H_

#include <functional>
#include <memory>

namespace voyager {

class Buffer;
class TcpConnection;

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void ()> ConnectFailureCallback;
typedef std::function<void (const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef std::function<void (const TcpConnectionPtr&,
                            Buffer*)> MessageCallback;
typedef std::function<void ()> TimerProcCallback;

}  // namespace voyager

#endif  // VOYAGER_CORE_CALLBACK_H_
