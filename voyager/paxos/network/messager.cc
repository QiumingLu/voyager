#include "voyager/paxos/network/messager.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Messager::Messager(Config* config, Network* network)
    : config_(config),
      network_(network) {
}

bool Messager::PackMessage(const PaxosMessage& msg, std::string* s) {
  std::string body;
  msg.SerializeToString(&body);
  s->append(body);
  return true;
}

bool Messager::UnPackMessage(const std::string& s, PaxosMessage* msg) {
  bool ret = msg->ParseFromArray(&*(s.data()), static_cast<int>(s.size()));
  return ret;
}

bool Messager::SendMessage(uint64_t node_id, const PaxosMessage& msg) {
  std::string s;
  bool ret = PackMessage(msg, &s);
  if (!ret) {
    return ret;
  }
  network_->SendMessage(NodeInfo(node_id), s);
  return true;
}

bool Messager::BroadcastMessage(const PaxosMessage& msg) {
  std::string s;
  bool ret = PackMessage(msg, &s);
  if (!ret) {
    return ret;
  }
  std::set<uint64_t>& nodes = config_->NodeIdSet();
  for (std::set<uint64_t>::iterator it = nodes.begin();
       it != nodes.end(); ++it) {
    network_->SendMessage(NodeInfo(*it), s);
  }
  return true;
}

bool Messager::BroadcastMessageToFollower(const PaxosMessage& msg) {
  return true;
}

}  // namespace paxos
}  // namespace voyager
