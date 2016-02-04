#ifndef MIRANTS_CORE_TCP_SERVER_H_
#define MIRANTS_CORE_TCP_SERVER_H_

#include <map>
#include <string>
#include <netdb.h>
#include <stdint.h>

#include "core/callback.h"
#include "port/atomic_sequence_num.h"
#include "util/scoped_ptr.h"

namespace mirants {

class Acceptor;
class EventLoop;
class SockAddr;

class TcpServer {
 public:
  TcpServer(EventLoop* eventloop,
            const SockAddr& addr,
            const std::string& name,
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

  void Start();

 private:
  void NewConnection(int sockfd, const struct sockaddr_storage& sa);
  void CloseConnection(const TcpConnectionPtr& conn_ptr);
  void CloseConnectionInLoop(const TcpConnectionPtr& conn_ptr);

  EventLoop* eventloop_;
  const struct addrinfo* servinfo_;
  scoped_ptr<Acceptor> acceptor_ptr_;
  const std::string name_;
  mirants::port::SequenceNumber sequence_num;

  ConnectionCallback connection_cb_;
  WriteCompleteCallback writecomplete_cb_;

  std::map<int, TcpConnectionPtr> connection_map_;
  
  // No copying allow
  TcpServer(const TcpServer&);
  void operator=(const TcpServer&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TCP_SERVER_H_
