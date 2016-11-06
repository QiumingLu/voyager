#include "voyager/paxos/node.h"

namespace voyager {
namespace paxos {

Node::Node() {
}

Status Node::Start() {
  return Status::OK();
}

Status Node::Propose() {
  return Status::OK();
}

Status Node::BatchPropose() {
  return Status::OK();
}

}  // namespace paxos
}  // namespace voyager
