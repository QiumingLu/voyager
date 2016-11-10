#include "voyager/paxos/group.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Group::Group(const Config* config) : instance_(config) {
}

}  // namespace paxos
}  // namespace voyager
