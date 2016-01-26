#include "port/singleton.h"
#include "util/testharness.h"

#include <string>
#include "port/currentthread.h"
#include "port/thread.h"

namespace mirants {

class SingletonTest {
 public:
  SingletonTest() {
    printf("tid=%d, constructing %p\n", port::CurrentThread::Tid(), this);
  }

  ~SingletonTest() {
    printf("tid=%d, destructing %p %s\n", 
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
  printf("tid=%d, %p name=%s\n",
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
  printf("tid=%d, %p name=%s\n",
         port::CurrentThread::Tid(),
         &(port::Singleton<SingletonTest>::Instance()),
         port::Singleton<SingletonTest>::Instance().name().c_str());
  port::Singleton<SingletonTest>::Shutdown();
}

}  // namespace mirants

int main(int argc, char** argv) {
  return mirants::test::RunAllTests();
}
