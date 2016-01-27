#include "port/threadlocalstorage_singleton.h"
#include <string>
#include <stdio.h>
#include "port/thread.h"
#include "port/currentthread.h"

namespace mirants {
namespace port {

class Test {
 public:
  Test() {
    printf("tid=%d, constructing %p\n", CurrentThread::Tid(), this);
  }

  ~Test() {
    printf("tid=%d, destructing %p %s\n", 
           CurrentThread::Tid(), this, name_.c_str());
  }

  const std::string& name() const { return name_; }
  void set_name(const std::string& n) { name_ = n; }
  
 private:
  std::string name_;
};

void ThreadFunc(const std::string& n) {
  printf("tid=%d, %p name=%s\n",
         CurrentThread::Tid(),
         &ThreadLocalStorageSingleton<Test>::Instance(),
         ThreadLocalStorageSingleton<Test>::Instance().name().c_str());
  ThreadLocalStorageSingleton<Test>::Instance().set_name(n);
  printf("tid=%d, %p name=%s\n",
         CurrentThread::Tid(),
         &ThreadLocalStorageSingleton<Test>::Instance(),
         ThreadLocalStorageSingleton<Test>::Instance().name().c_str());
}

}  // namespace port
}  // namespace mirants

int main(int argc, char** argv) {
  mirants::port::Thread t1(std::bind(mirants::port::ThreadFunc, "thread 1"));
  mirants::port::Thread t2(std::bind(mirants::port::ThreadFunc, "thread 2"));
  t1.Start();
  t2.Start();
  t1.Join();
  t2.Join();
  return 0;
}
