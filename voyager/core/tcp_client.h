#ifndef VOYAGER_CORE_TCP_CLIENT_H_
#define VOYAGER_CORE_TCP_CLIENT_H_

#include <memory>
#include <string>
#include <netdb.h>

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
  void DisConnect();

  void Close();

  EventLoop* GetLoop() const { return ev_; }

  void SetConnectionCallback(const ConnectionCallback& func) {
    connection_cb_ = func;
  }
void Retry(const TcpConnectionPtr& ptr); 
  void SetConnectionCallback(ConnectionCallback&& func) {
    connection_cb_ = std::move(func);
  }

  void SetCloseCallback(const CloseCallback& func) {
	close_cb_ = func;
  }

  void SetCloseCallback(CloseCallback&& func) {
	close_cb_ = std::move(func);
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
  
  std::string name_;
  std::string server_ipbuf_;
  EventLoop* ev_;
  ConnectorPtr connector_ptr_;
  uint32_t conn_id_;
  bool connect_;

  ConnectionCallback connection_cb_;
  CloseCallback close_cb_;
  MessageCallback message_cb_;
  WriteCompleteCallback writecomplete_cb_;

  TcpConnectionPtr conn_ptr_;

  // No copying allow
  TcpClient(const TcpClient&);
  void operator=(const TcpClient&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_CLIENT_H_
