#ifndef VOYAGER_PAXOS_BALLOT_NUMBER_H_
#define VOYAGER_PAXOS_BALLOT_NUMBER_H_

namespace voyager {
namespace paxos {

class BallotNumber {
 public:
  BallotNumber(uint64_t proposal_id, uint64_t node_id) :
      : proposal_id_(proposal_id), node_id_(node_id) {
  }

  bool operator>=(const BallotNumber& other) const;
  bool operator!=(const BallotNumber& other) const;
  bool operator==(const BallotNumber& other) const;
  bool operator>(const BallotNumber& other) const;

 private:
  uint64_t proposal_id_;
  uint64_t node_id_;
};

inline bool operator>=(const BallotNumber& other) const {
  if (proposal_id_ == other.proposal_id_) {
    return node_id_ >= other.node_id_;
  } else {
    return proposal_id_ > other.proposal_id_;
  }
}

inline bool operator!=(const BallotNumber& other) const {
  return (proposal_id_ != other.proposal_id_) || (node_id_ != other.node_id_);
}

inline bool operator==(const BallotNumber& other) const {
  return (proposal_id_ == other.proposal_id_) && (node_id_ == other.node_id_);
}

inline bool operator>(const BallotNumber& other) const {
  if (proposal_id_ == other.proposal_id_) {
    return node_id_ > other.node_id_;
  } else {
    return proposal_id_ > other.proposal_id_;
  }
}

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_BALLOT_NUMBER_H_
