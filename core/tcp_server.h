#ifndef MIRANTS_CORE_TCP_SERVER_H_
#define MIRANTS_CORE_TCP_SERVER_H_

#include <string>
#include <netdb.h>
#include <stdint.h>
#include "util/scoped_ptr.h"

namespace mirants {

class Acceptor;
class EventLoop;
class SockAddr;

class TcpServer {
 public:
  TcpServer(EventLoop* eventloop, const SockAddr& addr, int backlog = SOMAXCONN);
  ~TcpServer();

  void Start();

 private:
  EventLoop* eventloop_;
  const struct addrinfo* servinfo_;
  scoped_ptr<Acceptor> acceptor_ptr_;
  const std::string name_;

  // No copying allow
  TcpServer(const TcpServer&);
  void operator=(const TcpServer&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TCP_SERVER_H_
