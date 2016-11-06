#include "voyager/paxos/messager.h"

namespace voyager {
namespace paxos {

Messager::Messager(const Config* config) : config_(config) {
}

Status Messager::PackMessage(const PaxosMessage& msg, std::string* s) {
  return Status::OK();
}

Status Messager::UnPackMessage(const std::string& s, PaxosMessage* msg) {
  return Status::OK();
}

Status Messager::SendMessage(uint64_t node_id, const PaxosMessage& msg) {
  return Status::OK();
}

Status Messager::BroadcastMessage(const PaxosMessage& msg) {
  return Status::OK();
}

Status Messager::BroadcastMessageToFollower(const PaxosMessage& msg) {
  return Status::OK();
}

}  // namespace paxos
}  // namespace voyager
