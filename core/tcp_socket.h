#ifndef MIRANTS_CORE_TCP_SOCKET_H_
#define MIRANTS_CORE_TCP_SOCKET_H_

#include <sys/types.h>
#include <sys/socket.h>

namespace mirants {

class TcpSocket {
 public:
  explicit TcpSocket(int sockfd) : sockfd_(sockfd) { }
  ~TcpSocket();

  int SocketFd() const { return sockfd_; }

  void BindAddress(const struct sockaddr* addr);
  void Listen();
  void Accept();
  
  void SetReuseAddr(bool on);

  void SetReusePort(bool on);

  void SetKeepAlive(bool on);

  void SetTcpNoDelay(bool on);

 private:
  const int sockfd_;

  // No copying allow
  TcpSocket(const TcpSocket&);
  void operator=(const TcpSocket&);
};

}  // namespace mirants

#endif  // MIRANTS_CORE_TCP_SOCKET_H_
