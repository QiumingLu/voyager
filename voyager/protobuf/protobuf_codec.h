// Copyright (c) 2017 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_PROTOBUF_CODEC_H_
#define VOYAGER_PROTOBUF_CODEC_H_

#include <functional>
#include <memory>
#include <utility>

#include <voyager/core/buffer.h>
#include <voyager/core/tcp_connection.h>

namespace voyager {

enum ProtoCodecError {
  kOk,
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
  void SetErrorCallback(const ErrorCallback& cb) { error_cb_ = cb; }

  bool SendMessage(const TcpConnectionPtr& p, const Message& message) {
    bool res = false;
    if (p) {
      std::string s;
      char buf[kHeaderLen];
      memset(buf, 0, kHeaderLen);
      s.append(buf, kHeaderLen);
      res = message.AppendToString(&s);
      if (res) {
        int size = static_cast<int>(s.size());
        memcpy(buf, &size, kHeaderLen);
        s.replace(s.begin(), s.begin() + kHeaderLen, buf, kHeaderLen);
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
      if (buf->ReadableSize() >= kHeaderLen) {
        int size;
        memcpy(&size, buf->Peek(), kHeaderLen);
        if (buf->ReadableSize() >= static_cast<size_t>(size)) {
          std::unique_ptr<Message> message(new Message());
          res = message->ParseFromArray(buf->Peek() + kHeaderLen,
                                        size - kHeaderLen);
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
  static const int kHeaderLen = 4;

  MessageCallback message_cb_;
  ErrorCallback error_cb_;

  // No copying allowed
  ProtobufCodec(const ProtobufCodec&);
  void operator=(const ProtobufCodec&);
};

}  // namespace voyager

#endif  // VOYAGER_PROTOBUF_CODEC_H_
