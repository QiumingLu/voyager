#include "voyager/paxos/node.h"
#include "voyager/paxos/group.h"
#include "voyager/paxos/log_storage.h"
#include "voyager/paxos/messager.h"
#include "voyager/paxos/network.h"

namespace voyager {
namespace paxos {

Node::Node(const Options& options)
    : storage_(new LogStorage(options)),
      network_(new Network(options.node_info)),
      messager_(new Messager(network_)) {
  for (size_t i = 0; i < options.group_size; ++i) {
    Group* group = new Group(i, options, storage_, messager_);
    groups_.push_back(group);
  }
}

Node::~Node() {
  for (size_t i = 0; i < groups_.size(); ++i) {
    delete groups_[i];
  }
  delete messager_;
  delete network_;
  delete storage_;
}

void Node::Start() {
  network_->StartServer(
      std::bind(&Node::OnReceiveMessage, this, std::placeholders::_1));
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
