#include "voyager/paxos/acceptor.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Acceptor::Acceptor(const Config* config) {

}

void Acceptor::OnPrepare(const PaxosMessage& msg) {
}

void Acceptor::OnAccpet(const PaxosMessage& msg) {
}

}  // namespace paxos
}  // namespace voyager
