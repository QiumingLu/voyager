#include "voyager/paxos/group.h"
#include "voyager/util/logging.h"

namespace voyager {
namespace paxos {

Group::Group(int group_id, const Options& options, Network* network)
    : config_(group_id, options, network),
      instance_(&config_) {
}

bool Group::Start() {
  bool ret = config_.Init();
  if (ret) {
    ret = instance_.Init();
  }
  return ret;
}

bool Group::NewValue(const Slice& value,
                     MachineContext* context,
                     uint64_t* new_instance_id) {
  return instance_.NewValue(value, context, new_instance_id);
}

void Group::OnReceiveMessage(const Slice& s) {
  instance_.OnReceiveMessage(s);
}

}  // namespace paxos
}  // namespace voyager
