#ifndef MIRANTS_UTIL_ANY_H_
#define MIRANTS_UTIL_ANY_H_

namespace mirants {
namespace anyimpl {

  struct bad_any_cast {
  };

  struct empty_any {
  };

  struct base_any_policy {
  };

  template<typename T>
  struct typed_base_any_policy : base_any_policy {
  };

  template<typename T>
  struct small_any_policy : typed_base_any_policy<T> {
  };

  template<typename T>
  struct big_any_policy : typed_base_any_policy<T> {
  };

  template<typename T>
  struct choose_policy {
  };

  template<typename T>
  struct choose_policy<T*> {
  };

  struct any;

  template<>
  struct choose_policy<any> {
  };

  #define SMALL_POLICY(TYPE) template<> struct     \
    choose_policy<TYPE>  { };

  #undef SMALL_POLICY

  template<typename T>
  base_any_policy* get_policy() {
    static typename choose_policy<T>::type policy;
    return &policy;
  }

}

struct any {
 public:
  any() {
  }

  any(const char* t);

  template<typename T>
  any(const T& t);

  any(const any& t);

  ~any() {
  }

  template<typename T>
  any& assign(const T& t);

  any& assign(const any& t);

  any& operator=(const char* t);

  template<typename T>
  any& operator=(const T& t);

  any& swap(any& t);

  template<typename T>
  T& cast();

  bool empty() const;

  void reset();

  bool compatible(const any& t) const;

 private:
  anyimpl::base_any_policy* policy;
  void* object;
};

}  // namespace mirants

#endif  // MIRANTS_UTIL_ANY_H_
