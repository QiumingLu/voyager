#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Config::Config(size_t group_idx, const Options& options, Network* network)
    : group_idx_(group_idx),
      log_sync_(options.log_sync),
      sync_interval_(options.sync_interval),
      node_id_(options.node_info.GetNodeId()),
      node_size_(options.all_nodes.size()),
      follow_nodes_(options.follow_nodes),
      db_(new DB()),
      messager_(new Messager(this, network)),
      state_machine_(new StateMachineImpl(this)) {

  std::string temp(options.log_storage_path);
  if (temp[temp.size() - 1] != '/') {
    temp += '/';
  }
  char name[512];
  snprintf(name, sizeof(name), "%sg%zu", temp.c_str(), group_idx);
  log_storage_path_ = std::string(name);

  for (auto node : options.all_nodes) {
    node_id_set_.insert(node.GetNodeId());
  }
}

Config::~Config() {
  delete state_machine_;
  delete messager_;
  delete db_;
}

bool Config::Init() {
  int ret = db_->Open(group_idx_, log_storage_path_);
  if (ret != 0) {
    return false;
  }
  return state_machine_->Init();
}

bool Config::IsValidNodeId(uint64_t node_id) const {
  return true;
}

}  // namespace paxos
}  // namespace voyager
