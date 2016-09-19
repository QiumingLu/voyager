#ifndef VOYAGER_PORT_THREADLOCALSTORAGE_H_
#define VOYAGER_PORT_THREADLOCALSTORAGE_H_

#include <pthread.h>

namespace voyager {
namespace port {

template<typename T>
class ThreadLocalStorage {
 public:
  ThreadLocalStorage() {
    pthread_key_create(&key_, &ThreadLocalStorage::DeleteValue);
  }
  ~ThreadLocalStorage() {
    pthread_key_delete(key_);
  }

  T* GetValue() {
    T* value = static_cast<T*>(pthread_getspecific(key_));
    if (value == nullptr) {
      value = new T();
      pthread_setspecific(key_, value);
    }
    return value;
  }

 private:
  static void DeleteValue(void* value) {
    delete static_cast<T*>(value);
  }

  pthread_key_t key_;

  // No copying allowed
  ThreadLocalStorage(const ThreadLocalStorage&);
  void operator=(const ThreadLocalStorage&);
};

}  // namespace port
}  // namespace voyager

#endif  // VOYAGER_PORT_THREADLOCALSTORAGE_H_
