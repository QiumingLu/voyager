#include "port/threadlocalstorage.h"
#include <stdio.h>
#include "port/thread.h"
#include "port/currentthread.h"

namespace mirants {
namespace port {

class ThreadLocalStorageTest {
 public:
  ThreadLocalStorageTest() {
    printf("tid=%d, constructing %p\n", CurrentThread::Tid(), this);
  }

  ~ThreadLocalStorageTest() {
    printf("tid=%d, destructing %p\n", CurrentThread::Tid(), this);
  }
};

ThreadLocalStorage<ThreadLocalStorageTest> obj1;

void ThreadFunc() {
  obj1.GetValue();
}

}  // namespace port
}  // namespace mirants

int main(int argc, char** argv) {
  mirants::port::obj1.GetValue();
  mirants::port::Thread t(mirants::port::ThreadFunc);
  t.Start();
  t.Join();
  pthread_exit(0);
  return 0;
}
