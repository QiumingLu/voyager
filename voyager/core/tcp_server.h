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
            const std::string& name = std::string("MirantsServer"),
            int thread_size = 1,
            int backlog = SOMAXCONN);
  ~TcpServer();

  const std::string& name() const { return name_; }
  
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

  void Start();

 private:
  void NewConnection(int fd, const struct sockaddr_storage& sa);
  void CloseConnection(const TcpConnectionPtr& conn_ptr);
  void CloseConnectionInLoop(const TcpConnectionPtr& conn_ptr);

  EventLoop* eventloop_;
  std::string ipbuf_;
  scoped_ptr<Acceptor> acceptor_ptr_;
  const std::string name_;
  std::shared_ptr<EventLoopThreadPool> ev_pool_;
  port::SequenceNumber sequence_num_;
  int conn_id_;

  ConnectionCallback connection_cb_;
  WriteCompleteCallback writecomplete_cb_;
  MessageCallback message_cb_;

  std::map<std::string, TcpConnectionPtr> connection_map_;
  
  // No copying allow
  TcpServer(const TcpServer&);
  void operator=(const TcpServer&);
};

}  // namespace voyager

#endif  // VOYAGER_CORE_TCP_SERVER_H_
