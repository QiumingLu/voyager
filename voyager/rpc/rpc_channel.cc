// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/rpc/rpc_channel.h"

#include <string>

namespace voyager {

RpcChannel::RpcChannel(EventLoop* loop) : loop_(loop), micros_(0) {
  codec_.SetMessageCallback(std::bind(&RpcChannel::OnResponse, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
  codec_.SetErrorCallback(std::bind(&RpcChannel::OnError, this,
                                    std::placeholders::_1,
                                    std::placeholders::_2));
}

RpcChannel::~RpcChannel() {
  for (auto it : call_map_) {
    delete it.second.response;
    delete it.second.done;
    loop_->RemoveTimer(it.second.timer);
  }
}

void RpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                            google::protobuf::RpcController* controller,
                            const google::protobuf::Message* request,
                            google::protobuf::Message* response,
                            google::protobuf::Closure* done) {
  int id = num_.GetNext();
  RpcMessage msg;
  msg.set_id(id);
  msg.set_service_name(method->service()->full_name());
  msg.set_method_name(method->name());
  msg.set_data(request->SerializeAsString());

  if (codec_.SendMessage(conn_, msg)) {
    TimerId t;
    if (micros_ > 0) {
      t = loop_->RunAfter(micros_,
                          std::bind(&RpcChannel::TimeoutHandler, this, id));
    }
    port::MutexLock lock(&mutex_);
    call_map_[id] = CallData(response, done, t);
  } else {
    if (error_cb_) {
      error_cb_(ERROR_CODE_UNKNOWN);
    }
    delete response;
    delete done;
  }
}

void RpcChannel::OnMessage(const TcpConnectionPtr& p, Buffer* buf) {
  codec_.OnMessage(p, buf);
}

void RpcChannel::TimeoutHandler(int id) {
  {
    port::MutexLock lock(&mutex_);
    auto it = call_map_.find(id);
    if (it != call_map_.end()) {
      delete it->second.response;
      delete it->second.done;
      call_map_.erase(it);
    }
  }
  if (error_cb_) {
    error_cb_(ERROR_CODE_TIMEOUT);
  }
}

bool RpcChannel::OnResponse(const TcpConnectionPtr& p,
                            std::unique_ptr<RpcMessage> msg) {
  int id = msg->id();
  CallData data;
  {
    port::MutexLock lock(&mutex_);
    auto it = call_map_.find(id);
    if (it != call_map_.end()) {
      data = it->second;
      call_map_.erase(it);
    }
  }
  loop_->RemoveTimer(data.timer);

  if (msg->error() == ERROR_CODE_OK) {
    if (data.response) {
      data.response->ParseFromString(msg->data());
    }
    if (data.done) {
      data.done->Run();
    }
  } else {
    if (error_cb_) {
      error_cb_(msg->error());
    }
    delete data.done;
  }
  delete data.response;
  return true;
}

void RpcChannel::OnError(const TcpConnectionPtr& p, ProtoCodecError code) {
  if (code == kParseError) {
    p->ForceClose();
  }
}

}  // namespace voyager
