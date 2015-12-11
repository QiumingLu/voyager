#ifndef MIRANTS_PORT_THREAD_H_
#define MIRANTS_PORT_THREAD_H_

#include <functional>
#include <string>
#include <utility>
#include <pthread.h>
#include "port/atomic.h"

namespace mirants {
namespace port {

class Thread {
 public:
  typedef std::function<void()> ThreadFunc;
  explicit Thread(const ThreadFunc& func, const std::string& name = std::string());
  explicit Thread(ThreadFunc&& func, const std::string& name);
  ~Thread();

  void Start();
  void Join();

  bool Started() const { return started_; }
  bool Joined() const { return joined_; }
  pid_t Tid() const { return tid_; }
  const std::string& Name() const { return name_; }
  static int ThreadCreatedNum() { 
    int32_t desnum_;
    return mirants::port::AtomicGet(num_, desnum_);
  }

 private:
  void SetDefaultName();
  void PthreadCall(const char* label, int result);

  bool started_;
  bool joined_;
  pthread_t pthread_id_;
  pid_t tid_;
  ThreadFunc func_;
  std::string name_;
  ThreadFunc func_;
  static Atomic32 num_;

  // No copying allow
  Thread(const Thread&);
  void operator=(const Thread&);
};

}  // namespace port
}  // namespace mirants

#endif  // MIRANTS_PORT_THREAD_H_
