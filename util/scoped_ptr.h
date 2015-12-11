#ifndef MIRANTS_UTIL_SCOPED_PTR_H_
#define MIRANTS_UTIL_SCOPED_PTR_H_

#include <assert.h>
#include <stddef.h>

namespace mirants {

template<typename T> class scoped_ptr;
template<typename T> class scoped_array;

template<typename T>
class scoped_ptr {
 public:
  typedef T value_type;

  explicit scoped_ptr(T* p = NULL) : ptr_(p) { }
  ~scoped_ptr() {
    enum { type_must_be_complete = sizeof(T) };
    delete ptr_;
  }

  void reset(T* p = NULL) {
    if (p != ptr_) {
      enum { type_must_be_complete = sizeof(T) };
      delete ptr_;
      ptr_ = p;
    }
  }

  T& operator*() const {
    assert(ptr_ != NULL);
    return *ptr_;
  }

  T* operator->() const {
    assert(ptr_ != NULL);
    return ptr_;
  }

  T* get() const { return ptr_; }

  bool operator==(T* p) const { return ptr_ == p; }
  bool operator!=(T* p) const { return ptr_ != p; }

  void swap(scoped_ptr& p2) {
    T* temp = ptr_;
    ptr_ = p2.ptr_;
    p2.ptr_ = temp;
  }

  T* release() {
    T* retVal = ptr_;
    ptr_ = NULL;
    return retVal;
  }

 private:
  T* ptr_;

  // Forbid comparision of scoped_ptr types.
  template<typename T2> bool operator==(scoped_ptr<T2> const& p2) const;
  template<typename T2> bool operator!=(scoped_ptr<T2> const& p2) const;

  // No copying allow
  scoped_ptr(const scoped_ptr&);
  void operator=(const scoped_ptr&);
};

template<typename T>
class scoped_array {
 public:
  typedef T value_type;

  explicit scoped_array(T* p = NULL) : array_(p) { }
  ~scoped_array() {
    enum { type_must_be_complete = sizeof(T) };
    delete[] array_;
  }

  void reset(T* p = NULL) {
   if (p != array_) {
    enum { type_must_be_complete = sizeof(T) };
    delete[] array_;
    array_ = p;
   }
  }

  T& operator[](std::ptrdiff_t i) const {
    assert(i >= 0);
    assert(array_ != NULL);
    return array_[i];
  }

  T* get() const {
    return array_;
  }

  bool operator==(T* p) const { return array_ == p; }
  bool operator!=(T* p) const { return array_ != p; }

  void swap(scoped_array& p2) {
    T* temp = array_;
    array_ = p2.array_;
    p2.array_ = temp;
  }

  T* release() {
    T* retVal = array_;
    array_ = NULL;
    return retVal;
  }

 private:
  T* array_;

  // Forbid comparision of different scoped_array types.
  template<typename T2> bool operator==(scoped_array<T2> const& p2) const;
  template<typename T2> bool operator!=(scoped_array<T2> const& p2) const;

  // No copying allow
  scoped_array(const scoped_array&);
  void operator=(const scoped_array&);
};

}  // namespace mirants

#endif  // MIRANTS_UTIL_SCOPED_PTR_H_
