#ifndef VOYAGER_PAXOS_NETWORK_MESSAGER_H_
#define VOYAGER_PAXOS_NETWORK_MESSAGER_H_

#include <string>

#include "voyager/paxos/paxos.pb.h"
#include "voyager/paxos/network/network.h"

namespace voyager {
namespace paxos {

class Config;

class Messager {
 public:
  Messager(Config* config, Network* network);

  bool PackMessage(const PaxosMessage& msg, std::string* s);
  bool UnPackMessage(const std::string& s, PaxosMessage* msg);

  bool SendMessage(uint64_t node_id, const PaxosMessage& msg);
  bool BroadcastMessage(const PaxosMessage& msg);
  bool BroadcastMessageToFollower(const PaxosMessage& msg);

 private:
  Config* config_;
  Network* network_;

  // No copying allowed
  Messager(const Messager&);
  void operator=(const Messager&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_NETWORK_MESSAGER_H_
