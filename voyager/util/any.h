#ifndef VOYAGER_UTIL_ANY_H_
#define VOYAGER_UTIL_ANY_H_

#include <stddef.h>

#include <utility>
#include <stdexcept>

namespace voyager {
namespace anyimpl {

struct bad_any_cast {
};

struct empty_any {
};

struct base_any_policy {
  virtual void static_delete(void** t)  = 0;
  virtual void copy_from_value(void const* src, void** dest) = 0;
  virtual void clone(void* const* src, void** dest) = 0;
  virtual void move(void* const* src, void** dest) = 0;
  virtual void* get_value(void** src) = 0;
  virtual size_t get_size() = 0;
};

template<typename T>
struct typed_base_any_policy : base_any_policy {
  virtual size_t get_size() { return sizeof(T); }
};

template<typename T>
struct small_any_policy : typed_base_any_policy<T> {
  virtual void static_delete(void** t) { }
  virtual void copy_from_value(void const* src, void** dest) {
    new(dest) T(*reinterpret_cast<T const*>(src));
  }
  virtual void clone(void* const* src, void** dest) { *dest = *src; }
  virtual void move(void* const* src, void** dest) { *dest = *src; }
  virtual void* get_value(void** src) { return reinterpret_cast<void*>(src); }
};

template<typename T>
struct big_any_policy : typed_base_any_policy<T> {
  virtual void static_delete(void** t) {
    if (*t) {
      delete(*reinterpret_cast<T**>(t));
      *t = NULL;
    }
  }
  virtual void copy_from_value(void const* src, void** dest) {
    *dest = new T(*reinterpret_cast<T const*>(src));
  }
  virtual void clone(void* const* src, void** dest) {
    *dest = new T(**reinterpret_cast<T* const*>(src));
  }
  virtual void move(void* const* src, void** dest) {
    (*reinterpret_cast<T**>(dest))->~T();
    **reinterpret_cast<T**>(dest) = **reinterpret_cast<T* const*>(src);
  }
  virtual void* get_value(void** src) {
    return *src;
  }
};

template<typename T>
struct choose_policy {
  typedef big_any_policy<T> type;
};

template<typename T>
struct choose_policy<T*> {
  typedef small_any_policy<T*> type;
};

struct any;

// Choosing the policy for an any type is illegal, but should never happen.
// This is designed to throw a compiler error.
template<>
struct choose_policy<any> {
  typedef void type;
};

// Specializations for small types.
#define SMALL_POLICY(TYPE) template<> struct     \
  choose_policy<TYPE>  { typedef small_any_policy<TYPE> type; };

SMALL_POLICY(signed char);
SMALL_POLICY(unsigned char);
SMALL_POLICY(signed short);
SMALL_POLICY(unsigned short);
SMALL_POLICY(signed int);
SMALL_POLICY(unsigned int);
SMALL_POLICY(signed long);
SMALL_POLICY(unsigned long);
SMALL_POLICY(float);
SMALL_POLICY(bool);

#undef SMALL_POLICY

// This function will return a different policy for each type.
template<typename T>
base_any_policy* get_policy() {
  static typename choose_policy<T>::type policy;
  return &policy;
}

}

struct any {
 public:
  any()
      : policy(anyimpl::get_policy<anyimpl::empty_any>()), 
        object(NULL) {
  }

  any(const char* t)
      : policy(anyimpl::get_policy<anyimpl::empty_any>()),
        object(NULL) {
    assign(t);
  }

  template<typename T>
  any(const T& t)
      : policy(anyimpl::get_policy<anyimpl::empty_any>()),
        object(NULL) {
    assign(t);
  }

  any(const any& t)
      : policy(anyimpl::get_policy<anyimpl::empty_any>()),
        object(NULL) {
    assign(t);
  }

  ~any() {
    policy->static_delete(&object);
  }

  // Assignment function.
  template<typename T>
  any& assign(const T& t) {
    reset();
    policy = anyimpl::get_policy<T>();
    policy->copy_from_value(&t, &object);
    return *this;
  } 

  // Assignment function from another any.
  any& assign(const any& t) {
    reset();
    policy = t.policy;
    policy->clone(&t.object, &object);
    return *this;
  }

  // Assignment operator, specialed for literal strings.
  // They have types like const char[6] which don't work as expected.
  any& operator=(const char* t) {
    return assign(t);
  }

  // Assignmenet operator.
  template<typename T>
  any& operator=(const T& t) {
    return assign(t);
  }

  // Utility functions
  any& swap(any& t) {
    std::swap(policy, t.policy);
    std::swap(object, t.object);
    return *this;
  }

  // Cast operator. You can only cast to the original type.
  template<typename T>
  T& cast() {
    if (policy != anyimpl::get_policy<T>()) {
      throw anyimpl::bad_any_cast();
    }
    T* t = reinterpret_cast<T*>(policy->get_value(&object));
    return *t;
  }

  // Return true if the any contains no value
  bool empty() const {
    return policy == anyimpl::get_policy<anyimpl::empty_any>();
  }

  // Frees any allocated memory, and sets the value to NULL.
  void reset() {
    policy->static_delete(&object);
    policy = anyimpl::get_policy<anyimpl::empty_any>();
  }

  // Returns  true if the two types are the same.
  bool compatible(const any& t) const {
    return policy == t.policy;
  }

 private:
  anyimpl::base_any_policy* policy;
  void* object;
};

}  // namespace voyager

#endif  // VOYAGER_UTIL_ANY_H_
