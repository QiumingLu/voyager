#ifndef MIRANTS_ATOMIC_SEQUENCE_NUM_H_
#define MIRANTS_ATOMIC_SEQUENCE_NUM_H_

#include <atomic>

namespace mirants {
namespace port {

class SequenceNumber {
 public:

  SequenceNumber() : num_(0) { }

  int GetNext() {
    return num_++;
  }

 private:
  std::atomic<int> num_;
};

}  // namespace port
}  // namespace mirants

#endif  // MIRANTS_ATOMIC_SEQUENCE_NUM_H_
