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

Status Group::NewValue(const Slice& value, uint64_t* new_instance_id) {
  return instance_.NewValue(value, new_instance_id);
}

void Group::OnReceiveMessage(const Slice& s) {
  instance_.OnReceiveMessage(s);
}
}  // namespace paxos
}  // namespace voyager
