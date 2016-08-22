#ifndef VOYAGER_PORT_THREADLOCALSTORAGE_SINGLETON_H_
#define VOYAGER_PORT_THREADLOCALSTORAGE_SINGLETON_H_

#include <assert.h>
#include <pthread.h>

namespace voyager {
namespace port {

template<typename T>
class ThreadLocalStorageSingleton {
 public:
  static T& Instance() {
    if (!value_) {
      value_ = new T();
      helper_.set(value_);
    }
    return *value_;
  }

 private:
  class Helper {
   public:
    Helper() {
      pthread_key_create(&key_, &ThreadLocalStorageSingleton::Delete);
    }
    ~Helper() {
      pthread_key_delete(key_);
    }

    void set(T* value) {
      assert(pthread_getspecific(key_) == NULL);
      pthread_setspecific(key_, value);
    }

    pthread_key_t key_;
  };

  static void Delete(void* value) {
    delete static_cast<T*>(value);
  }

  static __thread T* value_;
  static Helper helper_;

  ThreadLocalStorageSingleton();
  ~ThreadLocalStorageSingleton();
  // No copying allow
  ThreadLocalStorageSingleton(const ThreadLocalStorageSingleton&);
  void operator=(const ThreadLocalStorageSingleton&);
};

template<typename T>
__thread T* ThreadLocalStorageSingleton<T>::value_ = NULL;

template<typename T>
typename ThreadLocalStorageSingleton<T>::Helper
    ThreadLocalStorageSingleton<T>::helper_;

}  // namespace port
}  // namespace voyager

#endif  // VOYAGER_PORT_THREADLOCALSTORAGE_SINGLETON_H_
