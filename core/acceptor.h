#ifndef MIRANTS_CORE_ACCEPTOR_H_
#define MIRANTS_CORE_ACCEPTOR_H_

#include <netdb.h>
#include "core/tcp_socket.h"
#include "core/dispatch.h"

namespace mirants {

class EventLoop;
class Acceptor {
  public:
  explicit Acceptor(EventLoop* eventloop, 
                    const struct addrinfo* addr, 
                    int backlog, bool reuseport = false);
  ~Acceptor();

  void EnableListen();
  void AcceptHandler();
  bool IsListenning() const { return listenning_; }

 private:
  EventLoop* eventloop_;
  TcpSocket tcpsocket_;
  int  backlog_;
  bool listenning_;
  Dispatch dispatch_;

  // No copying allow
  Acceptor(const Acceptor&);
  void operator=(const Acceptor&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_ACCEPTOR_H_
