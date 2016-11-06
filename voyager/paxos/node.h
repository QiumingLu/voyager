#ifndef VOYAGER_PAXOS_NODE_H_
#define VOYAGER_PAXOS_NODE_H_

#include "voyager/util/status.h"

namespace voyager {
namespace paxos {

class Node {
 public:
  Node();
  ~Node();

  Status Start();

  Status Propose();
  Status BatchPropose();

 private:

  // No copying allowed
  Node(const Node&);
  void operator=(const Node&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_NODE_H_
