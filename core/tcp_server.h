#ifndef MIRANTS_CORE_TCP_SERVER_H_
#define MIRANTS_CORE_TCP_SERVER_H_

#include <netdb.h>
#include <stdint.h>
#include "util/scoped_ptr.h"

namespace mirants {

class Acceptor;
class SockAddr;

class TcpServer {
 public:
  TcpServer(const SockAddr& addr);
  ~TcpServer();

 private:
  struct addrinfo* servinfo_;
  scoped_ptr<Acceptor> acceptor_ptr_;

  // No copying allow
  TcpServer(const TcpServer&);
  void operator=(const TcpServer&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TCP_SERVER_H_
