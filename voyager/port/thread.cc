#include "voyager/port/thread.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <sys/syscall.h>

#ifdef __linux__
#include <sys/prctl.h>
#endif

#include <algorithm>

#include "voyager/util/logging.h"
#include "voyager/port/currentthread.h"

namespace voyager {
namespace port {

namespace CurrentThread {

__thread uint64_t cached_tid = 0;
__thread const char* thread_name = "unknow";

}  // namespace CurrentThread

namespace {

#ifdef __linux__
uint64_t GetTid() {
  return static_cast<uint64_t>(::syscall(SYS_gettid));
}

#elif __APPLE__
uint64_t GetTid() {
  return static_cast<uint64_t>(pthread_mach_thread_np(pthread_self()));
}

#else
uint64_t GetTid() {
  pthread_t tid = pthread_self();
  uint64_t thread_id = 0;
  memcpy(&thread_id, &tid, std::min(sizeof(thread_id), sizeof(tid)));
  return thread_id;
}

#endif

void AfterFork() {
  CurrentThread::cached_tid = 0;
  CurrentThread::thread_name = "main";
  CurrentThread::Tid();
}

class ThreadInitializer {
 public:
  ThreadInitializer() {
    CurrentThread::thread_name = "main";
    CurrentThread::Tid();
    pthread_atfork(NULL, NULL, &AfterFork);
  }
};

ThreadInitializer thread_init;

struct StartThreadState {
  typedef voyager::port::Thread::ThreadFunc ThreadFunc;
  ThreadFunc thread_func;
  std::string thread_name;
  uint64_t* thread_tid;
  StartThreadState(const ThreadFunc& func,
                   const std::string& name,
                   uint64_t* tid)
      : thread_func(func),
        thread_name(name),
        thread_tid(tid)
  { }
};

void* StartThreadWrapper(void* arg) {
  StartThreadState* state = reinterpret_cast<StartThreadState*>(arg);
  CurrentThread::thread_name = state->thread_name.c_str();
  *(state->thread_tid) = CurrentThread::Tid();
  state->thread_func();
  CurrentThread::thread_name = "finished";
  delete state;
  return NULL;
}

}  // anonymous namespace

void CurrentThread::CacheTid() {
  if (cached_tid == 0) {
    cached_tid = GetTid();
  }
}

// bool CurrentThread::IsMainThread() {
//   return Tid() == static_cast<uint64_t>(::getpid());
// }


Thread::Thread(const ThreadFunc& func, const std::string& name)
     : started_(false),
       joined_(false),
       pthread_id_(0),
       tid_(0),
       func_(func),
       name_(name) {
  SetDefaultName();
}

std::atomic<int> Thread::num_;

Thread::Thread(ThreadFunc&& func, const std::string& name)
     : started_(false),
       joined_(false),
       pthread_id_(0),
       tid_(0),
       func_(std::move(func)),
       name_(std::move(name)) {
  SetDefaultName();
}

Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(pthread_id_);
  }
}

void Thread::SetDefaultName() {
  ++num_;
  if (name_.empty()) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer),
             "Thread %d", num_.load(std::memory_order_relaxed));
    name_ = buffer;
  }
}

void Thread::PthreadCall(const char* label, int result) {
  if (result != 0) {
    VOYAGER_LOG(FATAL) << label << strerror(result);
  }
}

void Thread::Start() {
  assert(!started_);
  started_ = true;
  StartThreadState* state = new StartThreadState(func_, name_, &tid_);
  PthreadCall("start thread: ",
              pthread_create(&pthread_id_, NULL, &StartThreadWrapper, state));
}

void Thread::Join() {
  assert(started_);
  assert(!joined_);
  joined_ = true;
  PthreadCall("join thread: ", pthread_join(pthread_id_, NULL));
}

}  // namespace port
}  // namespace voyager
