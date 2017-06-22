// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_UTIL_STL_UTIL_H_
#define VOYAGER_UTIL_STL_UTIL_H_

#include <string>

namespace voyager {

inline char* string_as_array(std::string* str) {
  return str->empty() ? nullptr : &*str->begin();
}

template <typename ForwardIterator>
void STLDeleteContainerPointers(ForwardIterator begin, ForwardIterator end) {
  while (begin != end) {
    ForwardIterator temp = begin;
    ++begin;
    delete *temp;
  }
}

template <typename T>
void STLDeleteElements(T* container) {
  if (!container) return;
  STLDeleteContainerPointers(container->begin(), container->end());
  container->clear();
}

template <typename T>
void STLDeleteValues(T* v) {
  if (!v) return;
  for (typename T::iterator it = v->begin(); it != v->end(); ++it) {
    delete it->second;
  }
  v->clear();
}

}  // namespace voyager

#endif  // VOYAGER_UTIL_STL_UTIL_H_
