#include "voyager/port/threadlocalstorage_singleton.h"
#include <string>
#include <stdio.h>
#include "voyager/port/thread.h"
#include "voyager/port/currentthread.h"

namespace voyager {
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
}  // namespace voyager

int main(int argc, char** argv) {
  voyager::port::Thread t1(std::bind(voyager::port::ThreadFunc, "thread 1"));
  voyager::port::Thread t2(std::bind(voyager::port::ThreadFunc, "thread 2"));
  t1.Start();
  t2.Start();
  t1.Join();
  t2.Join();
  return 0;
}
