// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_UTIL_MACROS_H_
#define VOYAGER_UTIL_MACROS_H_

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
// Define this to 1 if the code is compiled in C++11 mode; leave it
// undefined otherwise.  Do NOT define it to 0 -- that causes
// '#ifdef LANG_CXX11' to behave differently from '#if LANG_CXX11'.
#define LANG_CXX11 1
#endif

#if defined(__clang__) && defined(LANG_CXX11) && defined(__has_warning)
#if __has_feature(cxx_attributes) && __has_warning("-Wimplicit-fallthrough")
#define VOYAGER_FALLTHROUGH_INTENDED [[clang::fallthrough]]  // NOLINT
#endif
#endif

#if defined(__GNUC__)
#if __GNUC__ >= 7
#define VOYAGER_FALLTHROUGH_INTENDED [[fallthrough]]
#endif
#endif

#ifndef VOYAGER_FALLTHROUGH_INTENDED
#define VOYAGER_FALLTHROUGH_INTENDED \
  do {                               \
  } while (0)
#endif

#endif  // VOYAGER_UTIL_MACROS_H_
