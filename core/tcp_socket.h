#ifndef MIRANTS_CORE_TCP_SOCKET_H_
#define MIRANTS_CORE_TCP_SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>

namespace mirants {

class TcpSocket {
 public:
  explicit TcpSocket(int socketfd) : socketfd_(socketfd) { }
  ~TcpSocket();

  int SocketFd() const { return socketfd_; }

  void BindAddress(const struct sockaddr_in* local_sa4);
  void BindAddress(const struct sockaddr_in6* local_sa6);
  void Listen();
  void Accept(struct sockaddr_in* peer_sa4);
  void Accept(struct sockaddr_in6* peer_sa6);
  
  void SetReuseAddr(bool on);
  void SetReusePort(bool on);
  void SetKeepAlive(bool on);
  void SetTcpNoDelay(bool on);

 private:
  const int socketfd_;

  // No copying allow
  TcpSocket(const TcpSocket&);
  void operator=(const TcpSocket&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TCP_SOCKET_H_
