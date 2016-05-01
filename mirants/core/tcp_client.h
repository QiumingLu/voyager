#ifndef MIRANTS_CORE_TCP_CLIENT_H_
#define MIRANTS_CORE_TCP_CLIENT_H_

#include <memory>
#include <string>
#include <netdb.h>

#include "mirants/core/tcp_connection.h"
#include "mirants/port/mutex.h"
#include "mirants/port/mutexlock.h"

namespace mirants {

class EventLoop;
class Connector;
class SockAddr;

typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient {
 public:
  TcpClient(EventLoop* ev, const SockAddr& addr, 
            const std::string& name = "MrantsClient");
  ~TcpClient();

  void Connect();
  void DisConnect();
  void Stop();

  void EnableRetry() { retry_ = true; }
  bool IsRetry() const { return retry_; }

  TcpConnectionPtr ConnPtr() const {
    port::MutexLock l(&mu_);
    return conn_ptr_; 
  }

  EventLoop* GetLoop() const { return ev_; }

  void SetConnectionCallback(const ConnectionCallback& func) {
    connection_cb_ = func;
  }

  void SetConnectionCallback(ConnectionCallback&& func) {
    connection_cb_ = std::move(func);
  }

  void SetMessageCallback(const MessageCallback& func) {
    message_cb_ = func;
  }

  void SetMessageCallback(MessageCallback&& func) {
    message_cb_ = std::move(func);
  }

  void SetWriteCompleteCallback(const WriteCompleteCallback& func) {
    writecomplete_cb_ = func;
  }

  void SetWriteCompleteCallback(WriteCompleteCallback&& func) {
    writecomplete_cb_ = std::move(func);
  }

 private:
  void NewConnection(int socketfd);
  void CloseConnection(const TcpConnectionPtr& conn);

  std::string name_;
  EventLoop* ev_;
  ConnectorPtr connector_ptr_;
  int conn_id_;
  bool retry_;
  bool connect_;

  mutable port::Mutex mu_;
  TcpConnectionPtr conn_ptr_;

  ConnectionCallback connection_cb_;
  MessageCallback message_cb_;
  WriteCompleteCallback writecomplete_cb_;

  // No copying allow
  TcpClient(const TcpClient&);
  void operator=(const TcpClient&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TCP_CLIENT_H_
