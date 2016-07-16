#include "voyager/port/threadlocalstorage.h"
#include <stdio.h>
#include "voyager/port/thread.h"
#include "voyager/port/currentthread.h"

namespace voyager {
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
}  // namespace voyager

int main(int argc, char** argv) {
  voyager::port::obj1.GetValue();
  voyager::port::Thread t(voyager::port::ThreadFunc);
  t.Start();
  t.Join();
  pthread_exit(0);
  return 0;
}
