#include "mirants/util/logging.h"
#include "mirants/util/testharness.h"

namespace mirants {
  
class LoggingTest {
};

TEST(LoggingTest, Simple) {
  char v1[] = "test char logger";
  short v2 = -1;
  unsigned short v3 = 1;
  int v4 = -2;
  unsigned int v5 = 2;
  long v6 = -3;
  unsigned long v7 = 3;
  long long v8 = -4;
  unsigned long long v9 = 4;
  double v10 = 5.0;
  char* v11 = v1;
  Slice slice("test slice logger");
  std::string str("test string logger");
  Status st = Status::IOError("log: test IOError");

  MIRANTS_LOG(INFO) << v1;
  MIRANTS_LOG(INFO) << v2 << " " << v3 << " " << v4 << " " << v5 << " " <<  
    v6 << " " << v7 << " " <<  v8 << " " << v9 << " " << v10;
  MIRANTS_LOG(INFO) << v11;
  MIRANTS_LOG(INFO) << slice;
  MIRANTS_LOG(INFO) << str;
  MIRANTS_LOG(INFO) << st;

  MIRANTS_LOG(ERROR) << "A error message!";
  MIRANTS_LOG(FATAL) << "A fatal message!";

  char* p = v1;
  char* q = NULL;
  CHECK_NOTNULL(p);
  CHECK_NOTNULL(q);
}

}  // namespace mirants

int main(int argc, char** argv) {
  return mirants::test::RunAllTests();
}
