#include "voyager/paxos/learner.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Learner::Learner(const Config* config, Messager* messager)
    : config_(config), messager_(messager) {
}

}  // namespace paxos
}  // namespace voyager
