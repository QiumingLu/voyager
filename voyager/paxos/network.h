#ifndef VOYAGER_PAXOS_NETWORK_H_
#define VOYAGER_PAXOS_NETWORK_H_

#include <set>
#include <utility>

#include "voyager/core/bg_eventloop.h"
#include "voyager/core/eventloop.h"
#include "voyager/core/sockaddr.h"
#include "voyager/core/tcp_client.h"
#include "voyager/core/tcp_server.h"

namespace voyager {
namespace paxos {

class Network {
 public:
  Network();

  void Start(const SockAddr& addr);

  void SendMessage(const SockAddr& addr, const std::string& message);

 private:
  void SendMessageInLoop(const SockAddr& addr, const std::string& message);

  std::map<std::string, TcpConnectionPtr> connection_map_;
  std::unique_ptr<BGEventLoop> bg_loop_;
  EventLoop* loop_;
  std::unique_ptr<TcpServer> server_;

  // No copying allowed
  Network(const Network&);
  void operator=(const Network&);
};

}  // namespace paxos
}  // namespace voyager

#endif   // VOYAGER_PAXOS_NETWORK_H_
