#ifndef VOYAGER_PAXOS_OPTIONS_H_
#define VOYAGER_PAXOS_OPTIONS_H_

#include <stdint.h>
#include <string>

namespace voyager {
namespace paxos {

struct Options {
  std::string ip;
  uint16_t port;
  bool log_sync;
  int sync_interval;
  size_t group_size;
};

}  // namespace paxos
}  // namespace voyager

#endif   // VOYAGER_PAXOS_OPTIONS_H_
