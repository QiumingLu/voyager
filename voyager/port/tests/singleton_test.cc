#include "voyager/port/singleton.h"
#include "voyager/util/testharness.h"

#include <string>
#include <inttypes.h>

#include "voyager/port/currentthread.h"
#include "voyager/port/thread.h"

namespace voyager {

class SingletonTest {
 public:
  SingletonTest() {
    printf("tid=%" PRIu64", constructing %p\n", port::CurrentThread::Tid(), this);
  }

  ~SingletonTest() {
    printf("tid=%" PRIu64", destructing %p %s\n", 
           port::CurrentThread::Tid(), this, name_.c_str());
  }

  const std::string& name() const { return name_; }
  void setName(const std::string& n) { name_ = n; }

 private:
  std::string name_;

  SingletonTest(const SingletonTest&);
  void operator=(const SingletonTest&);
};

void ThreadFunc() {
  printf("tid=%" PRIu64", %p name=%s\n",
         port::CurrentThread::Tid(),
         &(port::Singleton<SingletonTest>::Instance()),
         port::Singleton<SingletonTest>::Instance().name().c_str());
  port::Singleton<SingletonTest>::Instance().setName("change singleton name");
}

TEST(SingletonTest, TestSingleton) {
  port::Singleton<SingletonTest>::Instance().setName("singleton test");
  port::Thread t(ThreadFunc);
  t.Start();
  t.Join();
  printf("tid=%" PRIu64", %p name=%s\n",
         port::CurrentThread::Tid(),
         &(port::Singleton<SingletonTest>::Instance()),
         port::Singleton<SingletonTest>::Instance().name().c_str());
  port::Singleton<SingletonTest>::Shutdown();
}

}  // namespace voyager

int main(int argc, char** argv) {
  return voyager::test::RunAllTests();
}
