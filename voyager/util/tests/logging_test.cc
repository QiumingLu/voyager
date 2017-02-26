// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/util/logging.h"
#include "voyager/util/testharness.h"

namespace voyager {
  
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

  VOYAGER_LOG(INFO) << v1;
  VOYAGER_LOG(INFO) << v2 << " " << v3 << " " << v4 << " " << v5 << " " <<  
    v6 << " " << v7 << " " <<  v8 << " " << v9 << " " << v10;
  VOYAGER_LOG(INFO) << v11;
  VOYAGER_LOG(INFO) << slice;
  VOYAGER_LOG(INFO) << str;
  VOYAGER_LOG(INFO) << st;

  VOYAGER_LOG(ERROR) << "A error message!";
  VOYAGER_LOG(FATAL) << "A fatal message!";

  char* p = v1;
  char* q = nullptr;
  CHECK_NOTNULL(p);
  CHECK_NOTNULL(q);
}

}  // namespace voyager

int main(int argc, char** argv) {
  return voyager::test::RunAllTests();
}
