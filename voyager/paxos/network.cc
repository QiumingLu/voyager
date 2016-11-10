#include "voyager/paxos/network.h"

namespace voyager {
namespace paxos {

Network::Network() {
}

void Network::Start(const SockAddr& addr) {
  bg_loop_.reset(new BGEventLoop());
  loop_ = bg_loop_->Loop();
  server_.reset(new TcpServer(loop_, addr));
  server_->Start();
}

void Network::SendMessage(const SockAddr& addr,
                          const std::string& message) {
  std::string ipbuf(addr.Ipbuf());
  auto it = connection_map_.find(ipbuf);
  if (it != connection_map_.end()) {
    it->second->SendMessage(message);
  } else {
    std::shared_ptr<TcpClient> client(new TcpClient(loop_, addr));
    client_set_.insert(client);
    client->SetConnectionCallback(
        [this, ipbuf, message](const TcpConnectionPtr& p) {
      connection_map_[ipbuf] = p;
      p->SendMessage(message);
    });
    client->SetConnectFailureCallback([this, client]() {
      client_set_.erase(client);
    });
    client->SetCloseCallback([this, ipbuf](const TcpConnectionPtr& p) {
      connection_map_.erase(ipbuf);
    });
    client->Connect();
  }
}

}  // namespace paxos
}  // namespace voyager
