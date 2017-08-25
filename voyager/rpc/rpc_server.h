// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_RPC_SERVER_H_
#define VOYAGER_RPC_SERVER_H_

#include <map>
#include <memory>
#include <string>

#include <google/protobuf/service.h>

#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/core/tcp_server.h"
#include "voyager/protobuf/protobuf_codec.h"
#include "voyager/rpc/rpc.pb.h"

namespace voyager {

class RpcServer {
 public:
  RpcServer(EventLoop* loop, const SockAddr& addr, int thread_size = 1);

  void Start();
  void RegisterService(google::protobuf::Service* service);

 private:
  bool OnRequest(const TcpConnectionPtr& p, std::unique_ptr<RpcMessage> msg);
  void OnError(const TcpConnectionPtr& p, ProtoCodecError code);
  void Done(google::protobuf::Message* response, TcpConnectionPtr p);

  std::map<std::string, google::protobuf::Service*> services_;
  ProtobufCodec<RpcMessage> codec_;
  TcpServer server_;

  // No copying allowed
  RpcServer(const RpcServer&);
  void operator=(const RpcServer&);
};

}  // namespace voyager

#endif  // VOYAGER_RPC_SERVER_H_
