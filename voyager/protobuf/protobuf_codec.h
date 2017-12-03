// Copyright (c) 2017 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_PROTOBUF_PROTOBUF_CODEC_H_
#define VOYAGER_PROTOBUF_PROTOBUF_CODEC_H_

#include <functional>
#include <memory>
#include <utility>

#include "voyager/core/buffer.h"
#include "voyager/core/tcp_connection.h"
#include "voyager/util/coding.h"

namespace voyager {

enum ProtoCodecError {
  kSerializeError,
  kParseError,
};

template <typename Message>
class ProtobufCodec {
 public:
  typedef std::function<bool(const TcpConnectionPtr&, std::unique_ptr<Message>)>
      MessageCallback;
  typedef std::function<void(const TcpConnectionPtr&, ProtoCodecError)>
      ErrorCallback;

  ProtobufCodec() {}

  void SetMessageCallback(const MessageCallback& cb) { message_cb_ = cb; }
  void SetMessageCallback(MessageCallback&& cb) { message_cb_ = std::move(cb); }
  void SetErrorCallback(const ErrorCallback& cb) { error_cb_ = cb; }
  void SetErrorCallback(ErrorCallback&& cb) { error_cb_ = std::move(cb); }

  bool SendMessage(const TcpConnectionPtr& p, const Message& message) const {
    bool res = false;
    if (p) {
      uint32_t size = kHeaderSize + message.ByteSizeLong();
      std::string s;
      s.reserve(size);
      PutFixed32(&s, size);
      res = message.AppendToString(&s);
      if (res) {
        p->SendMessage(std::move(s));
      } else if (error_cb_) {
        error_cb_(p, kSerializeError);
      }
    }
    return res;
  }

  void OnMessage(const TcpConnectionPtr& p, Buffer* buf) {
    bool res = true;
    while (res) {
      if (buf->ReadableSize() >= kHeaderSize) {
        uint32_t size = DecodeFixed32(buf->Peek());
        if (buf->ReadableSize() >= static_cast<size_t>(size)) {
          std::unique_ptr<Message> message(new Message());
          res = message->ParseFromArray(buf->Peek() + kHeaderSize,
                                        size - kHeaderSize);
          if (res) {
            if (message_cb_) {
              res = message_cb_(p, std::move(message));
            }
            buf->Retrieve(size);
          } else if (error_cb_) {
            error_cb_(p, kParseError);
          }
          continue;
        }
      }
      break;
    }
  }

 private:
  static const uint32_t kHeaderSize = 4;

  MessageCallback message_cb_;
  ErrorCallback error_cb_;

  // No copying allowed
  ProtobufCodec(const ProtobufCodec&);
  void operator=(const ProtobufCodec&);
};

}  // namespace voyager

#endif  // VOYAGER_PROTOBUF_PROTOBUF_CODEC_H_
