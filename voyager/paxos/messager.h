#ifndef VOYAGER_PAXOS_MESSAGER_H_
#define VOYAGER_PAXOS_MESSAGER_H_

#include <string>

#include "voyager/util/status.h"
#include "voyager/paxos/config.h"
#include "voyager/paxos/network.h"
#include "voyager/paxos/paxos_message.h"

namespace voyager {
namespace paxos {

class Config;

class Messager {
 public:
  Messager(const Config* config, Network* network);

  Status PackMessage(const PaxosMessage& msg, std::string* s);
  Status UnPackMessage(const std::string& s, PaxosMessage* msg);

  Status SendMessage(uint64_t node_id, const PaxosMessage& msg);
  Status BroadcastMessage(const PaxosMessage& msg);
  Status BroadcastMessageToFollower(const PaxosMessage& msg);

 private:
  const Config* config_;
  Network* network_;

  // No copying allowed
  Messager(const Messager&);
  void operator=(const Messager&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_MESSAGER_H_
