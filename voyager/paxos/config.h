#ifndef VOYAGER_PAXOS_CONFIG_H_
#define VOYAGER_PAXOS_CONFIG_H_

#include <stdint.h>
#include <stddef.h>

#include "voyager/paxos/storage.h"

namespace voyager {
namespace paxos {

class Config {
 public:
  Config(LogStorage* storage,
         bool log_sync,
         int sync_interval,
         uint64_t node_id,
         size_t group_idx,
         size_t group_size);

  bool LogSync() const { return log_sync_; }
  int SyncInterval() const { return sync_interval_; }

  uint64_t GetNodeId() const { return node_id_; }
  size_t GetNodeSize() const { return node_size_; }

  size_t GetGroupIdx() const { return group_idx_; }
  size_t GetGroupSize() const { return group_size_; }

  size_t GetMajoritySize() const { return node_size_/2; }


 private:
  bool log_sync_;
  int sync_interval_;

  uint64_t node_id_;
  size_t node_size_;
  size_t group_idx_;
  size_t group_size_;

  // No copying allowed
  Config(const Config&);
  void operator=(const Config&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_CONFIG_H_
