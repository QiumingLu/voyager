// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_PUBLIC_VERSION_H_
#define VOYAGER_PUBLIC_VERSION_H_

// Voyager uses semantic versioning, see http://semver.org/.

#define VOYAGER_MAJOR_VERSION 1
#define VOYAGER_MINOR_VERSION 0
#define VOYAGER_PATCH_VERSION 2

// VOYAGER_VERSION_SUFFIX is non-empty for pre-releases (e.g. "-alpha",
// "-alpha.1",
// "-beta", "-rc", "-rc.1")
#define VOYAGER_VERSION_SUFFIX ""

#define VOYAGER_STR_HELPER(x) #x
#define VOYAGER_STR(x) VOYAGER_STR_HELPER(x)

// e.g. "1.0.0" or "1.0.0-alpha".
#define VOYAGER_VERSION_STRING                                      \
  (VOYAGER_STR(VOYAGER_MAJOR_VERSION) "." VOYAGER_STR(              \
      VOYAGER_MINOR_VERSION) "." VOYAGER_STR(VOYAGER_PATCH_VERSION) \
       VOYAGER_VERSION_SUFFIX)

#endif  // VOYAGER_PUBLIC_VERSION_H_
