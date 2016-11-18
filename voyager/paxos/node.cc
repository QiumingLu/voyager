#include "voyager/paxos/node.h"
#include "voyager/paxos/group.h"
#include "voyager/paxos/storage/multi_db.h"
#include "voyager/paxos/network/messager.h"
#include "voyager/paxos/network/network.h"

namespace voyager {
namespace paxos {

Node::Node()
    : network_(nullptr),
      messager_(nullptr),
      multi_db_(nullptr) {
}

Node::~Node() {
  for (size_t i = 0; i < groups_.size(); ++i) {
    delete groups_[i];
  }
  delete multi_db_;
  delete messager_;
  delete network_;
}

void Node::Start(const Options& options) {
  network_ = new Network(options.node_info);
  messager_ = new Messager(network_);
  multi_db_ = new MultiDB();
  multi_db_->OpenAll(options.log_storage_path, options.group_size);
  for (size_t i = 0; i < options.group_size; ++i) {
    Group* group = new Group(i, options, multi_db_->GetDB(i), messager_);
    group->Start();
    groups_.push_back(group);
  }
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
