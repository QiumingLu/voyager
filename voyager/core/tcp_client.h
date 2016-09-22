#ifndef VOYAGER_CORE_TCP_CLIENT_H_
#define VOYAGER_CORE_TCP_CLIENT_H_

#include <atomic>
#include <memory>
#include <string>
#include <utility>

#include "voyager/core/tcp_connection.h"
#include "voyager/port/atomic_sequence_num.h"

namespace voyager {

class EventLoop;
class TcpConnector;
class SockAddr;

typedef std::shared_ptr<TcpConnector> TcpConnectorPtr;

class TcpClient {
 public:
  typedef std::function<void (const Status&)> ConnectFailureCallback;

  TcpClient(EventLoop* ev, const SockAddr& addr,
            const std::string& name = "VoyagerClient");
  ~TcpClient();

  void Connect();
  void Close();

  EventLoop* GetLoop() const { return ev_; }

  void SetConnectFailureCallback(const ConnectFailureCallback& cb) {
    failure_cb_ = cb;
  }
  void SetConnectionCallback(const ConnectionCallback& cb) {
    connection_cb_ = cb;
  }
  void SetCloseCallback(const CloseCallback& cb) {
    close_cb_ = cb;
  }
  void SetMessageCallback(const MessageCallback& cb) {
    message_cb_ = cb;
  }
  void SetWriteCompleteCallback(const WriteCompleteCallback& cb) {
    writecomplete_cb_ = cb;
  }
  void SetErrorCallback(const ErrorCallback& cb) {
    error_cb_ = cb;
  }

  void SetConnectFailureCallback(ConnectFailureCallback&& cb) {
    failure_cb_ = std::move(cb);
  }
  void SetConnectionCallback(ConnectionCallback&& cb) {
    connection_cb_ = std::move(cb);
  }
  void SetCloseCallback(CloseCallback&& cb) {
    close_cb_ = std::move(cb);
  }
  void SetMessageCallback(MessageCallback&& cb) {
    message_cb_ = std::move(cb);
  }
  void SetWriteCompleteCallback(WriteCompleteCallback&& cb) {
    writecomplete_cb_ = std::move(cb);
  }
  void SetErrorCallback(ErrorCallback&& cb) {
    error_cb_ = cb;
  }

 private:
  void NewConnection(const Status& st, int socketfd);

  std::string name_;
  std::string server_ipbuf_;
  EventLoop* ev_;
  TcpConnectorPtr connector_ptr_;
  std::atomic<bool> connect_;
  static port::SequenceNumber conn_id_;

  ConnectFailureCallback failure_cb_;
  ConnectionCallback connection_cb_;
  CloseCallback close_cb_;
  MessageCallback message_cb_;
  WriteCompleteCallback writecomplete_cb_;
  ErrorCallback error_cb_;

  std::weak_ptr<TcpConnection> weak_ptr_;

  // No copying allowed
  TcpClient(const TcpClient&);
  void operator=(const TcpClient&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_CLIENT_H_
