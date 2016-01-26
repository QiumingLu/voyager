#ifndef MIRANTS_CORE_ACCEPTOR_H_
#define MIRANTS_CORE_ACCEPTOR_H_

#include <netdb.h>
#include "core/tcp_socket.h"

namespace mirants {

class Acceptor {
 public:
  explicit Acceptor(const struct addrinfo* addr, 
                    int backlog, bool reuseport = false);
  ~Acceptor();

  void EnableListen();
  void AcceptTcpHandler();
  bool IsListenning() const { return listenning_; }

 private:
  TcpSocket tcpsocket_;
  int  backlog_;
  bool listenning_;

  // No copying allow
  Acceptor(const Acceptor&);
  void operator=(const Acceptor&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_ACCEPTOR_H_
