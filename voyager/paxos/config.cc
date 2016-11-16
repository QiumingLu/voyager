#include "voyager/paxos/config.h"

namespace voyager {
namespace paxos {

Config::Config(size_t group_idx, const Options& options,
               LogStorage* storage, Messager* messager)
    : storage_(storage),
      messager_(messager),
      log_sync_(options.log_sync),
      sync_interval_(options.sync_interval),
      node_id_(options.node_info.GetNodeId()),
      node_size_(options.all_nodes.size()),
      group_idx_(group_idx),
      group_size_(options.group_size),
      all_nodes_(options.all_nodes),
      follow_nodes_(options.follow_nodes) {
}

}  // namespace paxos
}  // namespace voyager
