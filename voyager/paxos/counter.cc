#include "voyager/paxos/counter.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Counter::Counter(const Config* config) : config_(config) {
}

void Counter::AddReceivedNode(uint64_t node_id) {
  if (received_nodes_.find(node_id) == received_nodes_.end()) {
    received_nodes_.insert(node_id);
  }
}

void Counter::AddRejector(uint64_t node_id) {
  if (rejectors_.find(node_id) == rejectors_.end()) {
    rejectors_.insert(node_id);
  }
}

void Counter::AddPromisorOrAcceptor(uint64_t node_id) {
  if (promisors_or_acceptors_.find(node_id) == promisors_or_acceptors_.end()) {
    promisors_or_acceptors_.insert(node_id);
  }
}

bool Counter::IsPassedOnThisRound() const {
  return false;
}

bool Counter::IsRejectedOnThisRound() const {
  return false;
}

bool Counter::IsReceiveAllOnThisRound() const {
  return false;
}

void Counter::StartNewRound() {
  received_nodes_.clear();
  rejectors_.clear();
  promisors_or_acceptors_.clear();
}

}  // namespace paxos
}  // namespace voyager
