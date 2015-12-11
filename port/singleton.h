#ifndef MIRANTS_PORT_SINGLETON_H_
#define MIRANTS_PORT_SINGLETON_H_

#include <assert.h>
#include <pthread.h>

namespace mirants {
namespace port {
  
template<typename T>
class Singleton {
 public:
  static T& Instance() {
    pthread_once(&once_, &Singleton<T>::Init);
    return *instance_;
  }

  static void Shutdown() {
    delete instance_;
    instance_ = NULL;
  }

 private:
  static void Init() {
    assert(instance_ == NULL);
    instance_ = new T();
  }

  static pthread_once_t once_;
  static T* instance_;

  // No copying allow
  Singleton(const Singleton&);
  void operator=(const Singleton&);
};

template<typename T>
pthread_once_t Singleton<T>::once_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::instance_ = NULL;

}  // namespace mirants

#endif  // MIRANTS_PORT_SINGLETON_H_
