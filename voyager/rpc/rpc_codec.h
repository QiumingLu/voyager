// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_RPC_CODEC_H_
#define VOYAGER_RPC_CODEC_H_

#include <string>
#include <google/protobuf/message.h>
#include "voyager/core/buffer.h"

namespace voyager {

class RpcCodec {
 public:
  RpcCodec() { }
  bool ParseFromBuffer(Buffer* buf,
                       google::protobuf::Message* message);
  bool SerializeToString(const google::protobuf::Message& msg,
                         std::string* s);
 private:
  static const int kHeaderSize = 4;

  // No copying allowed
  RpcCodec(const RpcCodec&);
  void operator=(const RpcCodec&);
};

}  // namespace voyager

#endif  // VOYAGER_RPC_CODEC_H_
