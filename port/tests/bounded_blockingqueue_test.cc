#include "port/bounded_blockingqueue.h"
#include <stdio.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include "port/countdownlatch.h"
#include "port/thread.h"
#include "port/currentthread.h"
#include "util/stringprintf.h"

namespace mirants {
namespace port {

using namespace std::placeholders;

class BoundedBlockingQueueTest {
 public:
  BoundedBlockingQueueTest(int threads_size)
      : queue_(18),
        latch_(threads_size),
        threads_(threads_size) {
    for (int i = 0; i < threads_size; ++i) {
      threads_.push_back(new Thread(
            std::bind(&BoundedBlockingQueueTest::ThreadFunc, this),
            StringPrintf("thread %d", i)));
    }
    for_each(threads_.begin(), threads_.end(), std::bind(&Thread::Start, _1));
  }

  void Task(int num) {
    printf("waiting for all threads start...\n");
    latch_.Wait();
    for (int i = 1; i <= num; ++i) {
      std::string task(StringPrintf("task %d", i));
      queue_.Put(task);
      printf("tid=%d, add task=%s, queue's size=%zd\n",
             CurrentThread::Tid(), task.c_str(), queue_.Size());
    }
  }

  void JoinAll() {
    for (size_t i = 0; i < threads_.size(); ++i) {
      queue_.Put("No task, stop!");
    }
    for_each(threads_.begin(), threads_.end(), std::bind(&Thread::Join, _1));
  }

 private:
  void ThreadFunc() {
    printf("tid=%d, %s started.\n",
           CurrentThread::Tid(), CurrentThread::ThreadName());
    latch_.CountDown();
    while (true) {
      std::string task(queue_.Take());
      if (task == "No task, stop!") {
        printf("All taskes have been done! Yelp!\n");
        printf("tid=%d, get task = %s, queue's size = %zd\n",
               CurrentThread::Tid(), task.c_str(), queue_.Size());
        break;
      }
    }
  }

  BoundedBlockingQueue<std::string> queue_;
  CountDownLatch latch_;
  boost::ptr_vector<Thread> threads_;
};

}  // namespace port
}  // namespace mirants
int main(int argc, char** argv) {
  printf("pid=%d, tid=%d\n", ::getpid(), mirants::port::CurrentThread::Tid());
  mirants::port::BoundedBlockingQueueTest t(4);
  t.Task(100);
  t.JoinAll();
  return 0;
}
