#include "voyager/paxos/committer.h"

namespace voyager {
namespace paxos {

Committer::Committer() {
}

Status Committer::NewValue(const std::string& value) {
  return Status::OK();
}

}  // namespace paxos
}  // namespace voyager
