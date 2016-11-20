#ifndef VOYAGER_PAXOS_LEARNER_H_
#define VOYAGER_PAXOS_LEARNER_H_

#include <stdint.h>

namespace voyager {
namespace paxos {

class Config;

class Learner {
 public:
  Learner(Config* config);

  void SetInstanceId(uint64_t instance_id) { instance_id_ = instance_id; }

 private:
  const Config* config_;

  uint64_t instance_id_;

  // No copying allowed
  Learner(const Learner&);
  void operator=(const Learner&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_LEARNER_H_
