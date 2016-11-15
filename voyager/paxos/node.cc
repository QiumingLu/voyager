#include "voyager/paxos/node.h"
#include "voyager/paxos/group.h"
#include "voyager/paxos/log_storage.h"
#include "voyager/paxos/network.h"

namespace voyager {
namespace paxos {

Node::Node(const Options* options)
    : options_(options),
      my_info_(options->ip, options->port),
      storage_(new LogStorage(options)),
      network_(new Network(options)),
      messager_(new Messager(network_)) {
}

Node::~Node() {
  for (size_t i = 0; i < groups_.size(); ++i) {
    delete groups_[i];
  }
  delete network_;
  delete storage_;
}

void Node::Start() {
  for (size_t i = 0; i < options_->group_size; ++i) {
    Group* group = new Group(options_, storage_, messager_,
                             my_info_.GetNodeId(), i, options_->group_size);
    groups_.push_back(group);
  }

  network_->Start(std::bind(&Node::OnReceiveMessage, this,
                           std::placeholders::_1, std::placeholders::_2));
}

Status Node::Propose(size_t group_idx, const Slice& value,
                     uint64_t *new_instance_id) {
  return groups_[group_idx]->NewValue(value, new_instance_id);
}

void Node::OnReceiveMessage(const Slice& s) {
  size_t group_idx;
  memcpy(&group_idx, s.data(), sizeof(size_t));
  return groups_[group_idx]->OnReceiveMessage(s);
}

}  // namespace paxos
}  // namespace voyager
