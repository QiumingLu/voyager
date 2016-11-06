#include "voyager/paxos/proposer.h"
#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Proposer::Proposer(const Config* config) : counter_(config) {
}

void Proposer::Prepare()
{
}

void Proposer::OnPrepareReply(const PaxosMessage& msg)
{
}

void Proposer::Accept() {
}

void Proposer::OnAccpetReply(const PaxosMessage& msg)
{
}

}  // namespace paxos
}  // namespace voyager
