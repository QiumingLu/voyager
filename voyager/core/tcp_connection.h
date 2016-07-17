#ifndef VOYAGER_CORE_TCP_CONNECTION_H_
#define VOYAGER_CORE_TCP_CONNECTION_H_

#include <string>
#include <netinet/in.h>
#include <netdb.h>

#include "voyager/core/buffer.h"
#include "voyager/core/callback.h"
#include "voyager/core/dispatch.h"
#include "voyager/core/tcp_socket.h"
#include "voyager/util/scoped_ptr.h"
#include "voyager/util/any.h"

namespace voyager {

class EventLoop;
class Slice;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
 public:
  TcpConnection(const std::string& name, EventLoop* ev, int fd);
  ~TcpConnection();

  void SetConnectionCallback(const ConnectionCallback& func) { 
    connection_cb_ = func; 
  }
  void SetConnectionCallback(ConnectionCallback&& func) {
    connection_cb_ = std::move(func);
  }
  void SetDisConnectionCallback(const DisConnectionCallback& func) { 
    disconnection_cb_ = func; 
  }
  void SetDisConnectionCallback(DisConnectionCallback&& func) {
    disconnection_cb_ = std::move(func);
  }
  void SetWriteCompleteCallback(const WriteCompleteCallback& func) {
    writecomplete_cb_ = func;
  }
  void SetWriteCompleteCallback(WriteCompleteCallback&& func) {
    writecomplete_cb_ = std::move(func);
  }
  void SetMessageCallback(const MessageCallback& func) {
    message_cb_ = func;
  }
  void SetMessageCallback(MessageCallback&& func) {
    message_cb_ = std::move(func);
  }

  EventLoop* GetLoop() const { return eventloop_; }
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

  // Internal use only
  void SetCloseCallback(const CloseCallback& func) {
    close_cb_ = func;
  }

  void SetCloseCallback(CloseCallback&& func) {
    close_cb_ = std::move(func);
  }
 
  void EstablishConnection();
  void CloseConnection(); 
 
 private:
  enum ConnectState {
    kDisconnected,
    kDisconnecting,
    kConnected,
    kConnecting
  };

  void StartReadInLoop();
  void StopReadInLoop();
  void ShutDownInLoop();
  void ForceCloseInLoop();
  void SendInLoop(const void* data, size_t size);

  void HandleRead();
  void HandleWrite();
  void HandleClose();
  void HandleError();

  const std::string name_;
  EventLoop* eventloop_;
  TcpSocket socket_;
  ConnectState state_;
  scoped_ptr<Dispatch> dispatch_;

  Buffer readbuf_;
  Buffer writebuf_;

  any context_;

  ConnectionCallback connection_cb_;
  DisConnectionCallback disconnection_cb_;
  WriteCompleteCallback writecomplete_cb_;
  MessageCallback message_cb_;
  CloseCallback close_cb_;
  
  // No copying allow
  TcpConnection(const TcpConnection&);
  void operator=(const TcpConnection&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_CONNECTION_H_
