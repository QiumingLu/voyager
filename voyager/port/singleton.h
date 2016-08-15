#ifndef VOYAGER_PORT_SINGLETON_H_
#define VOYAGER_PORT_SINGLETON_H_

#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

namespace voyager {
namespace port {
  
template<typename T>
class Singleton {
 public:
  static T& Instance() {
    pthread_once(&once_, &Singleton<T>::Init);
    return *instance_;
  }

 private:
 static void Init() {
    assert(instance_ == NULL);
    instance_ = new T();
    atexit([]() { 
        delete instance_; 
        instance_ = NULL;
    });
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

}  // namespace port
}  // namespace voyager

#endif  // VOYAGER_PORT_SINGLETON_H_
