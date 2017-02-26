// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/port/thread.h"
#include "voyager/port/currentthread.h"
#include "voyager/util/testharness.h"

#include <sys/types.h>
#include <unistd.h>
#include <inttypes.h>

namespace voyager {

void ThreadFunc() {
  printf("tid=%" PRIu64"\n", port::CurrentThread::Tid());
}

void ThreadFunc2(int x) {
  printf("tid=%" PRIu64", x=%d\n", port::CurrentThread::Tid(), x);
}

class Foo {
 public:
  explicit Foo(double x) : x_(x) { }
  
  void MemberFunc() {
    printf("tid=%" PRIu64", Foo::x_=%f\n",  port::CurrentThread::Tid(), x_);
  }

  void MemberFunc2(const std::string& text) {
    printf("tid=%" PRIu64", Foo::x_=%f, test=%s\n", 
           port::CurrentThread::Tid(), x_, text.c_str());
  }

 private:
  double x_;
};

class ThreadTest { };

TEST(ThreadTest, TestThread) {
  ASSERT_EQ(static_cast<uint64_t>(::getpid()), port::CurrentThread::Tid());
  printf("pid=%d, tid=%" PRIu64"\n", ::getpid(), port::CurrentThread::Tid());

  port::Thread t1(ThreadFunc);
  t1.Start();
  t1.Join();

  port::Thread t2(std::bind(ThreadFunc2, 42),
            "thread2");
  t2.Start();
  t2.Join();

  Foo foo(78.8);
  port::Thread t3(std::bind(&Foo::MemberFunc, &foo),
            "thread3");
  t3.Start();
  t3.Join();

  port::Thread t4(std::bind(&Foo::MemberFunc2, std::ref(foo),
            std::string("thread4")));
  t4.Start();
  t4.Join();
}

}  // namespace voyager

int main(int argc, char** argv) {
  return voyager::test::RunAllTests();
}
