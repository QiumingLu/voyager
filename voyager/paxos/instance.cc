#include "voyager/paxos/instance.h"

namespace voyager {
namespace paxos {

Instance::Instance(const Config* config)
    : config_(config),
      acceptor_(config),
      learner_(config),
      proposer_(config) {
}

}  // namespace paxos
}  // namespace voyager
