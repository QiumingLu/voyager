#ifndef VOYAGER_PAXOS_NODE_H_
#define VOYAGER_PAXOS_NODE_H_

#include <stdint.h>
#include <string>
#include <vector>

#include "voyager/paxos/group.h"
#include "voyager/paxos/nodeinfo.h"
#include "voyager/paxos/options.h"
#include "voyager/util/status.h"

namespace voyager {
namespace paxos {

class Node {
 public:
  Node(const Options* options);
  ~Node();

  void Start();

  Status Propose(uint64_t group_idx, const std::string& value,
                 uint64_t* instance_id);

  Status BatchPropose();

  const NodeInfo& MyNodeInfo() const { return my_info_; }

 private:
  void OnReceiveMessage(const char* s, size_t n);

  const Options* options_;
  NodeInfo my_info_;
  std::vector<Group*> groups_;

  LogStorage storage_;
  Network network_;

  // No copying allowed
  Node(const Node&);
  void operator=(const Node&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_NODE_H_
