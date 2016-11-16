#ifndef VOYAGER_PAXOS_CONFIG_H_
#define VOYAGER_PAXOS_CONFIG_H_

#include <stdint.h>
#include <stddef.h>
#include <vector>

#include "voyager/paxos/nodeinfo.h"
#include "voyager/paxos/options.h"

namespace voyager {
namespace paxos {

class LogStorage;
class Messager;

class Config {
 public:
  Config(size_t group_idx, const Options& options,
         LogStorage* storage, Messager* messager);

  LogStorage* GetStorage() const { return storage_; }
  Messager* GetMessager() const { return messager_; }

  bool LogSync() const { return log_sync_; }
  int SyncInterval() const { return sync_interval_; }

  uint64_t GetNodeId() const { return node_id_; }
  size_t GetNodeSize() const { return node_size_; }

  size_t GetGroupIdx() const { return group_idx_; }
  size_t GetGroupSize() const { return group_size_; }

  size_t GetMajoritySize() const { return node_size_/2; }


 private:
  LogStorage* storage_;
  Messager* messager_;
  bool log_sync_;
  int sync_interval_;

  uint64_t node_id_;
  size_t node_size_;
  size_t group_idx_;
  size_t group_size_;

  std::vector<NodeInfo> all_nodes_;
  std::vector<NodeInfo> follow_nodes_;

  // No copying allowed
  Config(const Config&);
  void operator=(const Config&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_CONFIG_H_
