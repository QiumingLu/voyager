#include "voyager/paxos/network/messager.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Messager::Messager(Config* config, Network* network)
    : config_(config),
      network_(network) {
}

bool Messager::PackMessage(ContentType type,
                           PaxosMessage* pmsg,
                           CheckPointMessage* cmsg,
                           std::string* s) {
  Content content;
  content.set_type(type);
  content.set_group_id(config_->GetGroupId());
  content.set_version(1);
  content.set_allocated_paxos_msg(pmsg);
  content.set_allocated_checkpoint_msg(cmsg);
  return content.SerializeToString(s);
}

bool Messager::SendMessage(uint64_t node_id, PaxosMessage* msg) {
  std::string s;
  bool ret = PackMessage(PAXOS_MESSAGE, msg, nullptr, &s);
  if (!ret) {
    return ret;
  }
  network_->SendMessage(NodeInfo(node_id), s);
  return true;
}

bool Messager::BroadcastMessage(PaxosMessage* msg) {
  std::string s;
  bool ret = PackMessage(PAXOS_MESSAGE, msg, nullptr, &s);
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

bool Messager::BroadcastMessageToFollower(PaxosMessage* msg) {
  return true;
}

}  // namespace paxos
}  // namespace voyager
