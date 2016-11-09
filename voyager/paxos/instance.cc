#include "voyager/paxos/instance.h"

namespace voyager {
namespace paxos {

Instance::Instance(const Config* config)
    : config_(config),
      acceptor_(config, this),
      learner_(config),
      proposer_(config, this) {
}

}  // namespace paxos
}  // namespace voyager
