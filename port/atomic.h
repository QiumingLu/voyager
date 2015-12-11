#ifndef MIRANTS_PORT_ATOMIC_H_
#define MIRANTS_PORT_ATOMIC_H_

#include <stdint.h>

namespace mirants {
namespace port {

typedef int32_t Atomic32;
typedef int64_t Atomic64;


#define AtomicIncr(var,count) __atomic_add_fetch(&var,(count),__ATOMIC_RELAXED)
#define AtomicDecr(var,count) __atomic_sub_fetch(&var,(count),__ATOMIC_RELAXED)
#define AtomicGet(var,dstvar) do { \
    dstvar = __atomic_load_n(&var,__ATOMIC_RELAXED); \
} while(0)


inline Atomic32 AtomicAddAndGet(volatile Atomic32* ptr, Atomic32 increment) {
  return __sync_add_and_fetch(ptr, increment);
}

inline Atomic32 AtomicSubAndGet(volatile Atomic32* ptr, Atomic32 decrement) {
  return __sync_sub_and_fetch(ptr, decrement);
}

inline Atomic32 AtomicGetAndAdd(volatile Atomic32* ptr, Atomic32 increment) {
  return __sync_fetch_and_add(ptr, increment);
}

inline Atomic32 AtomicGetAndSub(volatile Atomic32* ptr, Atomic32 decrement) {
  return __sync_fetch_and_sub(ptr, decrement);
}

inline Atomic32 AtomicCompareAndSwap(volatile Atomic32* ptr, 
                                     Atomic32 old_value,
                                     Atomic32 new_value) {
  return __sync_val_compare_and_swap(ptr, old_value, new_value);
}

inline Atomic32 AtomicGetAndWrite(volatile Atomic32* ptr, Atomic32 new_value) {
  return __sync_lock_test_and_set(ptr, new_value);
}

inline Atomic64 AtomicAddAndGet(volatile Atomic64* ptr, Atomic64 increment) {
  return __sync_add_and_fetch(ptr, increment);
}

inline Atomic64 AtomicSubAndGet(volatile Atomic64* ptr, Atomic64 decrement) {
  return __sync_sub_and_fetch(ptr, decrement);
}

inline Atomic64 AtomicGetAndAdd(volatile Atomic64* ptr, Atomic64 increment) {
  return __sync_fetch_and_add(ptr, increment);
}

inline Atomic64 AtomicGetAndSub(volatile Atomic64* ptr, Atomic64 decrement) {
  return __sync_fetch_and_sub(ptr, decrement);
}

inline Atomic64 AtomicCompareAndSwap(volatile Atomic64* ptr, 
                                     Atomic64 old_value,
                                     Atomic64 new_value) {
  __sync_val_compare_and_swap(ptr, old_value, new_value);
  return old_value;
}

inline Atomic64 AtomicGetAndWrite(volatile Atomic64* ptr, Atomic64 new_value) {
  return __sync_lock_test_and_set(ptr, new_value);
}

}  // namespace port
}  // namespace mirants

#endif  // MIRANTS_PORT_ATOMIC_H_
