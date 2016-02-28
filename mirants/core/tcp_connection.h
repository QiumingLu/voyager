#ifndef MIRANTS_CORE_TCP_CONNECTION_H_
#define MIRANTS_CORE_TCP_CONNECTION_H_

#include <string>
#include <netinet/in.h>
#include <netdb.h>

#include "mirants/core/buffer.h"
#include "mirants/core/callback.h"
#include "mirants/core/tcp_socket.h"
#include "mirants/util/scoped_ptr.h"

namespace mirants {

class Dispatch;
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
  void SetCloseCallback(const CloseCallback& func) {
    close_cb_ = func;
  }
  void SetCloseCallback(CloseCallback&& func) {
    close_cb_ = std::move(func);
  }

  EventLoop* GetLoop() const { return eventloop_; }
  const std::string& name() const { return name_; }

  void EstablishConnection();
  void DeleteConnection();
 
  void StartRead();
  void StopRead();
  void ShutDown();
  void ForceClose();

  std::string StateToString() const;

  void SendMessage(std::string&& message);
  void SendMessage(const Slice& message);
  void SendMessage(Buffer* message);

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

  ConnectionCallback connection_cb_;
  WriteCompleteCallback writecomplete_cb_;
  MessageCallback message_cb_;
  CloseCallback close_cb_;
  
  // No copying allow
  TcpConnection(const TcpConnection&);
  void operator=(const TcpConnection&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TCP_CONNECTION_H_
