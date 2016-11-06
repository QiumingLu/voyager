#ifndef VOYAGER_PAXOS_LEARNER_H_
#define VOYAGER_PAXOS_LEARNER_H_

namespace voyager {
namespace paxos {

class Config;

class Learner {
 public:
  Learner(const Config* config);

 private:

  // No copying allowed
  Learner(const Learner&);
  void operator=(const Learner&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_LEARNER_H_
