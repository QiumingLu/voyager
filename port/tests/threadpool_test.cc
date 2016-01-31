#include "port/threadpool.h"
#include <stdio.h>
#include "port/currentthread.h"
#include "port/countdownlatch.h"

namespace mirants {
namespace port {

void Print() {
  printf("tid=%d, threadname=%s\n", 
         CurrentThread::Tid(), CurrentThread::ThreadName());
}

void Test(int poolsize) {
  ThreadPool pool(poolsize);
  pool.Start();
  for (int i = 0; i < 1000; ++i) {
    pool.AddTask(Print);
  }
  CountDownLatch latch(1);
  pool.AddTask(std::bind(&CountDownLatch::CountDown, &latch));
  latch.Wait();
  pool.Stop();
  printf("test end!\n");
  printf("%zd\n", pool.TaskSize());
}

}  // namespace port
}  // namespace mirants

int main(int argc, char** argv) {
  mirants::port::Test(4);
  return 0;
}
