#ifndef MIRANTS_CORE_TCP_CONNECTION_H_
#define MIRANTS_CORE_TCP_CONNECTION_H_

#include "core/callback.h"

namespace mirants {

class EventLoop;

class TcpConnection {
 public:
  TcpConnection(EventLoop* eventloop);
  ~TcpConnection();

  void SetConnectionCallback(const ConnectionCallback& func) { 
    connection_cb_ = func; 
  }
  void SetConnectionCallback(ConnectionCallback&& func) {
    connection_cb_ = std::move(func);
  }
  void SetWriteCompleteCallback(const WriteCompleteCallback& func) {
    writecompletet_cb_ = func;
  }
  void SetWriteCompleteCallback(WriteCompleteCallback&& func) {
    writecompletet_cb_ = std::move(func);
  }

  void SetCloseCallback(const CloseCallback& func) {
    close_cb_ = func;
  }
  void SetCloseCallback(CloseCallback&& func) {
    close_cb_ = std::move(func);
  }

  int conn_id() const { return conn_id_; }

 private:
  enum ConnectState {
    kDisconnected,
    kDisconnecting,
    kConnected,
    kConnecting
  };

  ConnectionCallback connection_cb_;
  WriteCompleteCallback writecompletet_cb_;
  CloseCallback close_cb_;

  int conn_id_;

  // No copying allow
  TcpConnection(const TcpConnection&);
  void operator=(const TcpConnection&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TCP_CONNECTION_H_
