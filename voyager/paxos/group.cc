#include "voyager/paxos/group.h"

namespace voyager {
namespace paxos {

Group::Group(const Options* options,
             LogStorage* storage,
             Messager* messager,
             uint64_t node_id,
             size_t group_idx,
             size_t group_size)
    : config_(storage, messager, options->log_sync, options->sync_interval,
              node_id, group_idx, group_size),
      instance_(&config_) {
}

}  // namespace paxos
}  // namespace voyager
