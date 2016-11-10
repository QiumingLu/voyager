#include "voyager/paxos/node.h"

namespace voyager {
namespace paxos {

Node::Node(int group_count) : group_count_(group_count) {
}

Status Node::Start() {
  for (int i = 0; i < group_count_; ++i) {
//    Group* group = new Group();
//    groups_.push_back(group);
  }

  return Status::OK();
}

Status Node::Propose(uint64_t group_idx, uint64_t instance_id,
                     const std::string& value) {
  return Status::OK();
}

Status Node::BatchPropose() {
  return Status::OK();
}

Status Node::OnReceiveMessage(const char* buf, int len) {
  int group_idx = -1;
  memcpy(&group_idx, buf, sizeof(int));
  return groups_[group_idx]->GetInstance()->OnReceiveMessage(buf, len);
}

}  // namespace paxos
}  // namespace voyager
