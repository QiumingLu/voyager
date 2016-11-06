#ifndef VOYAGER_PAXOS_PAXOS_MESSAGE_H_
#define VOYAGER_PAXOS_PAXOS_MESSAGE_H_

#include <string>
#include <utility>

namespace voyager {
namespace paxos {

class PaxosMessage {
 public:
  enum MessageType {
    kPrepare = 0,
    kPrepareReply = 1,
    kAccept = 2,
    kAcceptReply = 3,
  };

  PaxosMessage();

  void set_message_type(MessageType type) { message_type_ = type; }
  MessageType message_type() const { return message_type_; }

  void set_instance_id(uint64_t instance_id) { instance_id_ = instance_id; }
  uint64_t instance_id() const { return instance_id_; }

  void set_proposal_id(uint64_t proposal_id) { proposal_id_ = proposal_id; }
  uint64_t proposal_id() const { return proposal_id_; }

  void set_node_id(uint64_t node_id) { node_id_ = node_id; }
  uint64_t node_id() const { return node_id_; }

  void set_value(const std::string& value) { value_ = value; }
  void set_value(std::string&& value) { value_ = std::move(value); }
  const std::string& value() const { return value_; }

  void set_reject_for_promised_id(uint64_t promisedid) {
    reject_for_promised_id_ = promisedid;
  }
  uint64_t reject_for_promised_id() const { return reject_for_promised_id_; }

  void set_preaccept_proposal_id(uint64_t id) { preaccept_proposal_id_ = id; }
  uint64_t preaccept_proposal_id() const { return preaccept_proposal_id_; }

  void set_preaccept_node_id(uint64_t id) { preaccept_node_id_ = id; }
  uint64_t preaccept_node_id() const { return preaccept_node_id_;}
 private:
  MessageType message_type_;
  uint64_t instance_id_;
  uint64_t proposal_id_;
  uint64_t node_id_;
  std::string value_;

  uint64_t reject_for_promised_id_;
  uint64_t preaccept_proposal_id_;
  uint64_t preaccept_node_id_;
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_PAXOS_MESSAGE_H_
