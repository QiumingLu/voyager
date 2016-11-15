#ifndef VOYAGER_PAXOS_GROUP_H_
#define VOYAGER_PAXOS_GROUP_H_

#include "voyager/paxos/config.h"
#include "voyager/paxos/instance.h"
#include "voyager/util/slice.h"

namespace voyager {
namespace paxos {

class LogStorage;
class Messager;

class Group {
 public:
  Group(const Options* options, LogStorage* storage, Messager* messager,
        uint64_t node_id, size_t group_idx, size_t group_size);

  Instance* GetInstance() { return &instance_; }

  Status NewValue(const Slice& value, uint64_t* new_instance_id);

  void OnReceiveMessage(const Slice& s);

 private:
  Config config_;
  Instance instance_;

  // No copying allowed
  Group(const Group&);
  void operator=(const Group&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_GROUP_H_
