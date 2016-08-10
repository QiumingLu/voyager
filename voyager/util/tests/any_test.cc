#include "voyager/util/any.h"
#include "voyager/util/testharness.h"
#include "voyager/util/slice.h"

namespace voyager {

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
  any c(Slice("sddssd"));
  Slice &s = c.cast<Slice>();
  ASSERT_EQ(s.data(), "sddssd");
}

}  // namespace voyager

int main() {
  return voyager::test::RunAllTests();
}
