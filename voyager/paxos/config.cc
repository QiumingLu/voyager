#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Config::Config(LogStorage* storage,
               bool log_sync,
               bool sync_interval,
               uint64_t node_id,
               size_t group_idx,
               size_t group_size)
    : log_sync_(log_sync),
      sync_interval_(sync_interval),
      node_id_(node_id),
      group_idx_(group_idx),
      group_size_(group_size) {
}

}  // namespace paxos
}  // namespace voyager
