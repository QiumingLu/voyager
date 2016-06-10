#include "mirants/util/any.h"
#include "mirants/util/testharness.h"

namespace mirants {

class  AnyTest { };

TEST(AnyTest, TestAny) {
  any a(42);
  ASSERT_EQ(a.cast<int>(), 42);
  a = 13;
  ASSERT_EQ(a.cast<int>(), 13);
  a = "hello";
  ASSERT_EQ(a.cast<const char*>(), "hello");
  a = std::string("123456789");
  ASSERT_EQ(a.cast<std::string>(), std::string("123456789"));
  int n = 42;
  a = &n;
  ASSERT_EQ(*a.cast<int*>(), 42);
  any b = true;
  ASSERT_EQ(b.cast<bool>(), true);
  std::swap(a, b);
  ASSERT_EQ(a.cast<bool>(), true);
  a.cast<bool>() = false;
  ASSERT_EQ(a.cast<bool>(), false);
}

}  // namespace mirants

int main() {
  return mirants::test::RunAllTests();
}
