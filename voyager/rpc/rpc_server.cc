// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/rpc/rpc_server.h"

namespace voyager {

RpcServer::RpcServer(EventLoop* loop, const SockAddr& addr, int thread_size)
    : server_(loop, addr, "RpcServer", thread_size) {
  codec_.SetMessageCallback(std::bind(&RpcServer::OnRequest, this,
                                      std::placeholders::_1,
                                      std::placeholders::_2));
  codec_.SetErrorCallback(std::bind(&RpcServer::OnError, this,
                                    std::placeholders::_1, 
                                    std::placeholders::_2));
  server_.SetMessageCallback(std::bind(&ProtobufCodec<RpcMessage>::OnMessage,
                                       &codec_, std::placeholders::_1,
                                       std::placeholders::_2));
}

void RpcServer::Start() { server_.Start(); }

void RpcServer::RegisterService(google::protobuf::Service* service) {
  const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();
  assert(desc != nullptr);
  services_[desc->full_name()] = service;
}

bool RpcServer::OnRequest(const TcpConnectionPtr& p,
                          std::unique_ptr<RpcMessage> msg) {
  ErrorCode error;
  auto it = services_.find(msg->service_name());
  if (it != services_.end()) {
    google::protobuf::Service* service = it->second;
    const google::protobuf::ServiceDescriptor* desc = service->GetDescriptor();
    const google::protobuf::MethodDescriptor* method =
        desc->FindMethodByName(msg->method_name());
    if (method) {
      google::protobuf::Message* request(
          service->GetRequestPrototype(method).New());
      if (request->ParseFromString(msg->data())) {
        google::protobuf::Message* response =
            service->GetResponsePrototype(method).New();
        p->SetContext(new int(msg->id()));
        service->CallMethod(
            method, nullptr, request, response,
            google::protobuf::NewCallback(this, &RpcServer::Done, response, p));
        error = ERROR_CODE_OK;
      } else {
        error = ERROR_CODE_INVALID_REQUEST;
      }
      delete request;
    } else {
      error = ERROR_CODE_INVALID_METHOD;
    }
  } else {
    error = ERROR_CODE_INVALID_SERVICE;
  }
  if (error != ERROR_CODE_OK) {
    RpcMessage reply_msg;
    reply_msg.set_id(msg->id());
    reply_msg.set_error(error);
    codec_.SendMessage(p, reply_msg);
  }
  return true;
}

void RpcServer::OnError(const TcpConnectionPtr& p, ProtoCodecError code) {
  if (code == kParseError) {
    p->ForceClose();
  }
}

void RpcServer::Done(google::protobuf::Message* response, TcpConnectionPtr p) {
  int* id = reinterpret_cast<int*>(p->Context());
  RpcMessage msg;
  msg.set_id(*id);
  msg.set_data(response->SerializeAsString());
  codec_.SendMessage(p, msg);
  delete id;
  delete response;
}

}  // namespace voyager
