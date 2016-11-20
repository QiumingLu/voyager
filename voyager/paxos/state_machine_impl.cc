#include "voyager/paxos/state_machine_impl.h"
#include "voyager/paxos/config.h"
#include "voyager/util/logging.h"

namespace voyager {
namespace paxos {

StateMachineImpl::StateMachineImpl(Config* config)
    : config_(config) {
}

bool StateMachineImpl::Execute(size_t group_idx, uint64_t instance_id,
                                const std::string& value,
                                MachineContext* context) {
  SystemVariables variables;
  bool ret = variables.ParseFromArray(&*(value.data()),
                                       static_cast<int>(value.size()));
  if (!ret) {
  }
  if (context != nullptr && context->context != nullptr) {
  }

  return true;
}

bool StateMachineImpl::Init() {
  bool res = true;

  std::string s;
  int success = config_->GetDB()->GetSystemVariables(&s);
  if (success != 0 && success != 1) { return false; }

  std::set<uint64_t>& node_id_set = config_->NodeIdSet();

  if (success == 0) {
    res = variables_.ParseFromArray(&*(s.data()), static_cast<int>(s.size()));
    if (!res) {
      VOYAGER_LOG(ERROR) << "SystemVariablesSM::Init - "
                         << "variables_.ParseFromArray failed, s = " << s;
      return res;
    }
    node_id_set.clear();
    for (int i = 0; i < variables_.node_id_size(); ++i) {
      node_id_set.insert(variables_.node_id(i));
    }
  } else {
    variables_.set_gid(0);
    variables_.set_version(-1);

    for (std::set<uint64_t>::iterator it = node_id_set.begin();
         it != node_id_set.end(); ++it) {
      variables_.add_node_id(*it);
    }
  }
  return res;
}

}  // namespace paxos
}  // namespace voyager
