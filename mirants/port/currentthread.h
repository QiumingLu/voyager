#ifndef MIRANTS_PORT_CURRENT_THREAD_H_
#define MIRANTS_PORT_CURRENT_THREAD_H_

namespace mirants {
namespace port {
namespace CurrentThread {

extern __thread int  cached_tid;
extern __thread const char* thread_name;
extern void CacheTid();

inline int Tid() {
  if (__builtin_expect(cached_tid == 0, 0)) {
    CacheTid();
  }
  return cached_tid;
}

inline const char* ThreadName() {
  return thread_name;
}

extern bool IsMainThread();

}  // namespace CurrentThread
}  // namespace port
}  // namespace mirants

#endif  // MIRANTS_PORT_CURRENT_THREAD_H_
