#include "voyager/paxos/node.h"

namespace voyager {
namespace paxos {

Node::Node(const Options* options)
  : options_(options),
    my_info_(options->ip, options->port),
    storage_(options),
    network_(options) {
}

Node::~Node() {
  for (size_t i = 0; i < groups_.size(); ++i) {
    delete groups_[i];
  }
}

void Node::Start() {
  for (size_t i = 0; i < options_->group_size; ++i) {
    Group* group = new Group(options_, &storage_, &network_,
                             my_info_.GetNodeId(), i, options_->group_size);
    groups_.push_back(group);
  }

  network_.Start(std::bind(&Node::OnReceiveMessage, this,
                           std::placeholders::_1, std::placeholders::_2));
}

Status Node::Propose(size_t group_idx, const std::string& value,
                     uint64_t *instance_id) {
  return groups_[group_idx]->GetInstance()->NewValue(value, instance_id);
}

Status Node::BatchPropose() {
  return Status::OK();
}

void Node::OnReceiveMessage(const char* s, size_t n) {
  size_t group_idx;
  memcpy(&group_idx, s, sizeof(size_t));
  return groups_[group_idx]->GetInstance()->OnReceiveMessage(s, n);
}

}  // namespace paxos
}  // namespace voyager
