#ifndef VOYAGER_PAXOS_CONFIG_H_
#define VOYAGER_PAXOS_CONFIG_H_

#include <stdint.h>

namespace voyager {
namespace paxos {

class Config {
 public:
  Config();

  uint64_t GetNodeId() const { return node_id_; }

 private:
  uint64_t node_id_;

  // No copying allowed
  Config(const Config&);
  void operator=(const Config&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_CONFIG_H_
