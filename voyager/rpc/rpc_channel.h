// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_RPC_CHANNEL_H_
#define VOYAGER_RPC_CHANNEL_H_

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>

#include <google/protobuf/service.h>

#include "voyager/core/buffer.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/protobuf/protobuf_codec.h"
#include "voyager/rpc/rpc.pb.h"

namespace voyager {

class RpcChannel : public google::protobuf::RpcChannel {
 public:
  typedef std::function<void(ErrorCode code)> ErrorCallback;

  explicit RpcChannel(EventLoop* loop);
  virtual ~RpcChannel();

  void SetTcpConnectionPtr(const TcpConnectionPtr& p) { conn_ = p; }

  void SetTimeout(uint64_t ms) { ms_ = ms; }

  void SetErrorCallback(const ErrorCallback& cb) { error_cb_ = cb; }

  virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller,
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response,
                          google::protobuf::Closure* done);

  void OnMessage(const TcpConnectionPtr& p, Buffer* buf);

 private:
  struct CallData {
    google::protobuf::Message* response;
    google::protobuf::Closure* done;
    TimerId timer;

    CallData() : response(nullptr), done(nullptr), timer() {}
    CallData(google::protobuf::Message* r, google::protobuf::Closure* d,
             TimerId t)
        : response(r), done(d), timer(t) {}
  };

  void OnTimeout(int id);
  bool OnResponse(const TcpConnectionPtr& p, std::unique_ptr<RpcMessage> msg);
  void OnError(const TcpConnectionPtr& p, ProtoCodecError code);

  EventLoop* loop_;
  uint64_t ms_;
  ProtobufCodec<RpcMessage> codec_;
  TcpConnectionPtr conn_;
  ErrorCallback error_cb_;
  std::atomic<int> seq_;
  std::mutex mutex_;
  std::unordered_map<int, CallData> call_map_;

  // No copying allowed
  RpcChannel(const RpcChannel&);
  void operator=(const RpcChannel&);
};

}  // namespace voyager

#endif  // VOYAGER_RPC_CHANNEL_H_
