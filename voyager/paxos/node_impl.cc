#include "voyager/paxos/node_impl.h"
#include "voyager/paxos/group.h"
#include "voyager/paxos/nodeinfo.h"
#include "voyager/util/logging.h"
#include "voyager/util/stl_util.h"

namespace voyager {
namespace paxos {

NodeImpl::NodeImpl(const Options& options)
    : options_(options),
      network_(options.node_info) {
}

NodeImpl::~NodeImpl() {
  STLDeleteValues(&groups_);
}

bool NodeImpl::StartWorking() {
  bool ret = true;
  for (int i = 0; i < options_.group_size; ++i) {
    Group* group = new Group(i, options_, &network_);
    ret = group->Start();
    if (ret) {
      groups_[i] = group;
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

bool NodeImpl::Propose(int group_id, const Slice& value,
                       uint64_t *new_instance_id) {
  assert(groups_.find(group_id) != groups_.end());
  return groups_[group_id]->NewValue(value, nullptr, new_instance_id);
}

void NodeImpl::OnReceiveMessage(const Slice& s) {
  int group_id;
  memcpy(&group_id, s.data(), sizeof(int));
  if (groups_.find(group_id) != groups_.end()) {
    Slice new_s(s.data() + sizeof(int), s.size() - sizeof(int));
    if (!new_s.empty()) {
      groups_[group_id]->OnReceiveMessage(new_s);
    }
  } else {
    VOYAGER_LOG(ERROR) << "NodeImpl::OnReceiveMessage - group_id="
                       << group_id << "is not right!";
  }
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
