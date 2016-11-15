#ifndef VOYAGER_PAXOS_NODE_H_
#define VOYAGER_PAXOS_NODE_H_

#include <stdint.h>
#include <string>
#include <vector>

#include "voyager/paxos/nodeinfo.h"
#include "voyager/paxos/options.h"
#include "voyager/util/slice.h"
#include "voyager/util/status.h"

namespace voyager {
namespace paxos {

class Group;
class LogStorage;
class Network;
class Messager;

class Node {
 public:
  Node(const Options* options);
  ~Node();

  void Start();

  Status Propose(size_t group_idx, const Slice& value,
                 uint64_t* new_instance_id);

  const NodeInfo& MyNodeInfo() const { return my_info_; }

 private:
  void OnReceiveMessage(const Slice& s);

  const Options* options_;
  NodeInfo my_info_;
  LogStorage* storage_;
  Network* network_;
  Messager* messager_;

  std::vector<Group*> groups_;

  // No copying allowed
  Node(const Node&);
  void operator=(const Node&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_NODE_H_
