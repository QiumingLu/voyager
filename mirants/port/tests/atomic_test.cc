#include "mirants/port/atomic.h"
#include "mirants/util/testharness.h"

namespace mirants {

class AtomicTest { };

TEST(AtomicTest, TestAtomic) {
  port::Atomic32 a;
  port::AtomicGetAndWrite(&a, 1);
  ASSERT_EQ(2, port::AtomicIncr(&a, 1));
  ASSERT_EQ(1, port::AtomicDecr(&a, 1));
  ASSERT_EQ(1, port::AtomicGet(&a));
  ASSERT_EQ(2, port::AtomicAddAndGet(&a, 1));
  ASSERT_EQ(1, port::AtomicSubAndGet(&a, 1));
  ASSERT_EQ(1, port::AtomicGetAndAdd(&a, 1));
  ASSERT_EQ(2, port::AtomicGetAndSub(&a, 1));
  ASSERT_EQ(1, port::AtomicCompareAndSwap(&a, 1, 3));
  ASSERT_EQ(3, port::AtomicGetAndWrite(&a, 5));

  port::Atomic64 b;
  port::AtomicGetAndWrite(&b, 1);
  ASSERT_EQ(2, port::AtomicAddAndGet(&b, 1));
  ASSERT_EQ(1, port::AtomicSubAndGet(&b, 1));
  ASSERT_EQ(1, port::AtomicGetAndAdd(&b, 1));
  ASSERT_EQ(2, port::AtomicGetAndSub(&b, 1));
  ASSERT_EQ(1, port::AtomicCompareAndSwap(&b, 1, 3));
  ASSERT_EQ(3, port::AtomicGetAndWrite(&b, 5));
}

}  // namespace mirants

int main(int argc, char** argv) {
 return mirants::test::RunAllTests();
} 
