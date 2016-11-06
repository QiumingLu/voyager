#include "voyager/paxos/base.h"

namespace voyager {
namespace paxos {

Base::Base(const Config* config) : config_(config) {
}

Status Base::PackMessage(const PaxosMessage& msg, std::string* s) {
  return Status::OK();
}

Status Base::UnPackMessage(const std::string& s, PaxosMessage* msg) {
  return Status::OK();
}

Status Base::SendMessage(uint64_t node_id, const PaxosMessage& msg) {
  return Status::OK();
}

Status Base::BroadcastMessage(const PaxosMessage& msg) {
  return Status::OK();
}

Status Base::BroadcastMessageToFollower(const PaxosMessage& msg) {
  return Status::OK();
}

}  // namespace paxos
}  // namespace voyager
