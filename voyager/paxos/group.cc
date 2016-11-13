#include "voyager/paxos/group.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Group::Group(const Options* options,
             LogStorage* storage,
             Network* network,
             uint64_t node_id,
             size_t group_idx,
             size_t group_size)
    : config_(storage, options->log_sync, options->sync_interval,
              node_id, group_idx, group_size),
      messager_(&config_, network),
      instance_(&config_, &messager_) {
}

}  // namespace paxos
}  // namespace voyager
