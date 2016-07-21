#ifndef VOYAGER_CORE_TCP_SERVER_H_
#define VOYAGER_CORE_TCP_SERVER_H_

#include <map>
#include <string>
#include <netdb.h>

#include "voyager/core/tcp_connection.h"
#include "voyager/core/sockaddr.h"
#include "voyager/port/atomic_sequence_num.h"
#include "voyager/util/scoped_ptr.h"

namespace voyager {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer {
 public:
  TcpServer(EventLoop* eventloop,
            const SockAddr& addr,
            const std::string& name = std::string("VoyagerServer"),
            int thread_size = 1,
            int backlog = SOMAXCONN);
  ~TcpServer();

  const std::string& name() const { return name_; }

  // 连接建立之后的回调
  void SetConnectionCallback(const ConnectionCallback& func) {
    connection_cb_ = func;
  }
  void SetConnectionCallback(ConnectionCallback&& func) {
    connection_cb_ = std::move(func);
  }

  // 连接关闭之后的回调
  void SetCloseCallback(const CloseCallback& func) {
    close_cb_ = func;
  }
  void SetCloseCallback(CloseCallback&& func) {
    close_cb_ = std::move(func);
  }

  // 发送完消息之后的回调
  void SetWriteCompleteCallback(const WriteCompleteCallback& func) {
    writecomplete_cb_ = func;
  }
  void SetWriteCompleteCallback(WriteCompleteCallback&& func) {
    writecomplete_cb_ = std::move(func);
  }

  // 接收消息到来的回调
  void SetMessageCallback(const MessageCallback& func) {
    message_cb_ = func;
  }
  void SetMessageCallback(MessageCallback&& func) {
    message_cb_ = std::move(func);
  }

  void Start();

 private:
  void NewConnection(int fd, const struct sockaddr_storage& sa);

  EventLoop* eventloop_;
  std::string ipbuf_;
  const std::string name_;
  scoped_ptr<Acceptor> acceptor_ptr_;
  std::shared_ptr<EventLoopThreadPool> ev_pool_;
  port::SequenceNumber seq_;
  uint64_t conn_id_;

  ConnectionCallback connection_cb_;
  CloseCallback close_cb_;
  WriteCompleteCallback writecomplete_cb_;
  MessageCallback message_cb_;

  // No copying allow
  TcpServer(const TcpServer&);
  void operator=(const TcpServer&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_SERVER_H_
