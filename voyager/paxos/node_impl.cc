#include "voyager/paxos/node_impl.h"
#include "voyager/paxos/group.h"
#include "voyager/paxos/nodeinfo.h"
#include "voyager/util/logging.h"

namespace voyager {
namespace paxos {

NodeImpl::NodeImpl(const Options& options)
    : options_(options),
      network_(options.node_info) {
}

NodeImpl::~NodeImpl() {
  for (size_t i = 0; i < groups_.size(); ++i) {
    delete groups_[i];
  }
}

bool NodeImpl::StartWorking() {
  bool ret = true;
  for (size_t i = 0; i < options_.group_size; ++i) {
    Group* group = new Group(i, options_, &network_);
    ret = group->Start();
    if (ret) {
      groups_.push_back(group);
    } else {
      return ret;
    }
  }
  VOYAGER_LOG(DEBUG) << "Node::Start - " << "Group Start Successfully!";

  network_.StartServer(
      std::bind(&NodeImpl::OnReceiveMessage, this, std::placeholders::_1));
  VOYAGER_LOG(DEBUG) << "Node::Start - " << "Network StartServer Successfully!";

  return ret;
}

bool NodeImpl::Propose(size_t group_idx, const Slice& value,
                       uint64_t *new_instance_id) {
  return groups_[group_idx]->NewValue(value, new_instance_id);
}

void NodeImpl::OnReceiveMessage(const Slice& s) {
  size_t group_idx;
  memcpy(&group_idx, s.data(), sizeof(size_t));
  return groups_[group_idx]->OnReceiveMessage(s);
}

bool Node::Start(const Options& options, Node** nodeptr) {
  *nodeptr = nullptr;
  NodeImpl* impl = new NodeImpl(options);
  bool ret = impl->StartWorking();
  if (ret) {
    *nodeptr = impl;
  } else {
    delete impl;
  }
  return ret;
}

}  // namespace paxos
}  // namespace voyager
