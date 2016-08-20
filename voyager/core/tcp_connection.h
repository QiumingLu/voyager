#ifndef VOYAGER_CORE_TCP_CONNECTION_H_
#define VOYAGER_CORE_TCP_CONNECTION_H_

#include "voyager/core/buffer.h"
#include "voyager/core/callback.h"
#include "voyager/core/base_socket.h"
#include "voyager/util/scoped_ptr.h"
#include "voyager/util/any.h"

namespace voyager {

class Dispatch;
class EventLoop;
class Slice;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(const std::string& name, EventLoop* ev, int fd);
  ~TcpConnection();

  void SetConnectionCallback(const ConnectionCallback& cb) { 
    connection_cb_ = cb; 
  }
  void SetCloseCallback(const CloseCallback& cb) { 
    close_cb_ = cb; 
  }
  void SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writecomplete_cb_ = cb;
  }
  void SetMessageCallback(const MessageCallback& cb) {
    message_cb_ = cb;
  }
  
  void SetConnectionCallback(ConnectionCallback&& cb) {
    connection_cb_ = std::move(cb);
  }
  void SetCloseCallback(CloseCallback&& cb) {
    close_cb_ = std::move(cb);
  }
  void SetWriteCompleteCallback(WriteCompleteCallback&& cb) {
    writecomplete_cb_ = std::move(cb);
  }
  void SetMessageCallback(MessageCallback&& cb) {
    message_cb_ = std::move(cb);
  }

  EventLoop* OwnerLoop() const { return eventloop_; }
  const std::string& name() const { return name_; }

  void StartRead();
  void StopRead();
  void ShutDown();
  void ForceClose();

  void SendMessage(std::string&& message);
  void SendMessage(const Slice& message);
  void SendMessage(Buffer* message);

  std::string StateToString() const;

  bool IsDisConnected() const { return state_ == kDisconnected; }
  bool IsDisConnecting() const { return state_ == kDisconnecting; }
  bool IsConnected() const { return state_ == kConnected; }
  bool IsConnecting() const { return state_ == kConnecting; }

  // 默认为ture
  void SetTcpNoDelay(bool on) { socket_.SetTcpNoDelay(on); }

  void SetContext(const any& context) {
    context_ = context;
  }

  const any& Context() const {
    return context_;
  }

  any* MutableContext() {
    return &context_;
  }

  // Internal use only, use in TcpClient and TcpServer.
  void StartWorking();
 
 private:
  enum ConnectState {
    kDisconnected,
    kDisconnecting,
    kConnected,
    kConnecting
  };

  void SendInLoop(const void* data, size_t size);

  void HandleRead();
  void HandleWrite();
  void HandleClose();
  void HandleError();

  const std::string name_;
  EventLoop* eventloop_;
  BaseSocket socket_;
  ConnectState state_;
  scoped_ptr<Dispatch> dispatch_;

  Buffer readbuf_;
  Buffer writebuf_;

  any context_;

  ConnectionCallback connection_cb_;
  CloseCallback close_cb_;
  WriteCompleteCallback writecomplete_cb_;
  MessageCallback message_cb_;
  
  // No copying allow
  TcpConnection(const TcpConnection&);
  void operator=(const TcpConnection&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_CONNECTION_H_
