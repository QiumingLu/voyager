#include "voyager/paxos/network.h"
#include "voyager/core/buffer.h"

namespace voyager {
namespace paxos {

Network::Network(const Options* options) : addr_(options->ip, options->port) {
}

void Network::Start(const std::function<void (const char* s, size_t n)>& cb) {
  bg_loop_.reset(new BGEventLoop());
  loop_ = bg_loop_->Loop();
  server_.reset(new TcpServer(loop_, addr_));
  server_->SetMessageCallback([&cb](const TcpConnectionPtr&, Buffer* buf) {
    cb(buf->Peek(), buf->ReadableSize());
    buf->RetrieveAll();
  });

  server_->Start();
}

void Network::Stop() {
  loop_->Exit();
}

void Network::SendMessage(const NodeInfo& other, const std::string& message) {
  SockAddr addr(other.GetIP(), other.GetPort());
  loop_->RunInLoop(std::bind(&Network::SendMessageInLoop, this, addr, message));
}

void Network::SendMessageInLoop(const SockAddr& addr,
                                const std::string& message) {
  std::string ipbuf(addr.Ipbuf());
  auto it = connection_map_.find(ipbuf);
  if (it != connection_map_.end()) {
    it->second->SendMessage(message);
  } else {
    TcpClient* client(new TcpClient(loop_, addr));
    client->SetConnectionCallback(
        [this, ipbuf, message](const TcpConnectionPtr& p) {
      connection_map_[ipbuf] = p;
      p->SendMessage(message);
    });

    client->SetConnectFailureCallback([this, client]() {
      delete client;
    });

    client->SetCloseCallback([this, ipbuf, client](const TcpConnectionPtr& p) {
      connection_map_.erase(ipbuf);
      delete client;
    });

    client->Connect(false);
  }
}

}  // namespace paxos
}  // namespace voyager
