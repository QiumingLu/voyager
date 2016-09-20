#ifndef VOYAGER_PAXOS_COUNTER_H_
#define VOYAGER_PAXOS_COUNTER_H_

namespace voyager {
namespace paxos {

class Counter {
 public:
  Counter();

 private:

  // No copying allowed
  Counter(const Counter&);
  void operator=(const Counter&);
};

}  // namespace paxos
}  // namespace voyager

#endif  // VOYAGER_PAXOS_COUNTER_H_
