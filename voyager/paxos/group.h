#ifndef VOYAGER_PAXOS_GROUP_H_
#define VOYAGER_PAXOS_GROUP_H_

#include "voyager/paxos/config.h"
#include "voyager/paxos/messager.h"
#include "voyager/paxos/instance.h"

namespace voyager {
namespace paxos {

class Group {
 public:
  Group(const Options* options, LogStorage* storage, Network* network,
        uint64_t node_id, size_t group_idx, size_t group_size);

  Instance* GetInstance() { return &instance_; }

 private:
  Config config_;
  Messager messager_;
  Instance instance_;

  // No copying allowed
  Group(const Group&);
  void operator=(const Group&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_GROUP_H_
