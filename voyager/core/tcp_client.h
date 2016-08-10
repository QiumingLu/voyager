#ifndef VOYAGER_CORE_TCP_CLIENT_H_
#define VOYAGER_CORE_TCP_CLIENT_H_

#include "voyager/core/tcp_connection.h"

namespace voyager {

class EventLoop;
class Connector;
class SockAddr;

typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient {
 public:
  TcpClient(EventLoop* ev, const SockAddr& addr, 
            const std::string& name = "VoyagerClient");
  ~TcpClient();

  void Connect();
  void ReConnect();
  void Close();

  EventLoop* GetLoop() const { return ev_; }

  void EnableRetry() { retry_ = true; }
  
  bool IsRetry() const { return retry_; }

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

 private:
  void NewConnection(int socketfd);
  void HandleClose(const TcpConnectionPtr& ptr);

  std::string name_;
  std::string server_ipbuf_;
  EventLoop* ev_;
  ConnectorPtr connector_ptr_;
  uint32_t conn_id_;
  bool connect_;
  bool retry_;

  ConnectionCallback connection_cb_;
  CloseCallback close_cb_;
  MessageCallback message_cb_;
  WriteCompleteCallback writecomplete_cb_;

  std::weak_ptr<TcpConnection> weak_ptr_;

  // No copying allow
  TcpClient(const TcpClient&);
  void operator=(const TcpClient&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_CLIENT_H_
