#include "port/singleton.h"
#include "util/testharness.h"

namespace mirants {

class SingletonTest { };

TEST(SingletonTest, TestSingleton) {
}

}  // namespace mirants

int main(int argc, char** argv) {
  return mirants::test::RunAllTests();
}
