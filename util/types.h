#ifndef MIRANTS_UTIL_TYPES_H_
#define MIRANTS_UTIL_TYPES_H_

namespace mirants {

template<typename To, typename From>
inline To implicit_cast(From const &f) {
  return f;
}

template<typename To, typename From> 
inline To down_cast(From* f) {
  if (false) {
    implicit_cast<From*, To>(0);
  }
  return static_cast<To>(f);
}

}

#endif  // MIRANTS_UTIL_TYPES_H_
