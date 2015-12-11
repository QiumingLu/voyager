#ifndef MIRANTS_ATOMIC_SEQUENCE_NUM_H_
#define MIRANTS_ATOMIC_SEQUENCE_NUM_H_

#include <stdint.h> 

namespace mirants {
namespace port {

class SequenceNumber {
 public:
  typedef intptr_t AtomicInt;

  SequenceNumber() : num_(0) { }

  AtomicInt GetNext() {
    return __atomic_add_fetch(&word_, 1, __ATOMIC_RELAXED) - 1;
  }

 private:
  AtomicInt num_;
};

}  // namespace port
}  // namespace mirants

#endif  // MIRANTS_ATOMIC_SEQUENCE_NUM_H_
