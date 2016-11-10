#ifndef VOYAGER_PAXOS_NODE_H_
#define VOYAGER_PAXOS_NODE_H_
#include <stdint.h>
#include <string>
#include <vector>

#include "voyager/paxos/group.h"
#include "voyager/util/status.h"

namespace voyager {
namespace paxos {

class Node {
 public:
  Node(int group_count);
  ~Node();

  Status Start();

  Status Propose(uint64_t group_idx, uint64_t instance_id,
                 const std::string & value);
  Status BatchPropose();

  Status OnReceiveMessage(const char* buf, int len);
 private:
  int group_count_;
  std::vector<Group*> groups_;

  // No copying allowed
  Node(const Node&);
  void operator=(const Node&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_NODE_H_
