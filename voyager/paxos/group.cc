#include "voyager/paxos/group.h"

namespace voyager {
namespace paxos {

Group::Group(size_t group_idx, const Options& options,
             LogStorage* storage, Messager* messager)
    : config_(group_idx, options, storage, messager),
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
