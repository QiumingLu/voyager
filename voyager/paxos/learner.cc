#include "voyager/paxos/learner.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Learner::Learner(Config* config)
    : config_(config) {
}

}  // namespace paxos
}  // namespace voyager
