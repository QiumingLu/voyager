#include "mirants/port/thread.h"

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <sys/syscall.h>

#ifndef __MACH__
#include <linux/unistd.h>
#include <sys/prctl.h>
#endif

#include "mirants/util/logging.h"
#include "mirants/port/currentthread.h"

namespace mirants {
namespace port {

namespace CurrentThread {

__thread int  cached_tid = 0;
__thread const char* thread_name = "unknow";

}  // namespace CurrentThread

namespace {

#ifdef __linux__
pid_t GetTid() {
  return static_cast<pid_t>(::syscall(SYS_gettid));
}
#elif __APPLE__
pid_t GetTid() {
  return static_cast<pid_t>(pthread_mach_thread_np(pthread_self()));
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
  typedef mirants::port::Thread::ThreadFunc ThreadFunc;
  ThreadFunc thread_func;
  std::string thread_name;
  pid_t* thread_tid;
  StartThreadState(const ThreadFunc& func,
                   const std::string& name,
                   pid_t* tid) 
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

bool CurrentThread::IsMainThread() {
  return Tid() == ::getpid();
}

Atomic32 Thread::num_;

Thread::Thread(const ThreadFunc& func, const std::string& name)
     : started_(false),
       joined_(false),
       pthread_id_(0),
       tid_(0),
       func_(func),
       name_(name) 
{
  SetDefaultName();
}

Thread::Thread(ThreadFunc&& func, const std::string& name) 
     : started_(false),
       joined_(false),
       pthread_id_(0),
       tid_(0),
       func_(std::move(func)),
       name_(std::move(name))
{
  SetDefaultName();
}

Thread::~Thread() {
  if (started_ && !joined_) {
    pthread_detach(pthread_id_);
  }
}

void Thread::SetDefaultName() {
  int num = mirants::port::AtomicIncr(&num_, 1);
  if (name_.empty()) {
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "Thread %d", num);
    name_ = buffer;
  }
}

void Thread::PthreadCall(const char* label, int result) {
  if (result != 0) {
    MIRANTS_LOG(FATAL) << label << strerror(result);
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
}  // namespace mirants
