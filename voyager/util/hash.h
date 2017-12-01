// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef VOYAGER_UTIL_HASH_H_
#define VOYAGER_UTIL_HASH_H_

#include <stdint.h>
#include <string>

namespace voyager {

extern uint32_t Hash32(const char* data, size_t n, uint32_t seed);
extern uint64_t Hash64(const char* data, size_t n, uint64_t seed);

inline uint32_t Hash32(const char* data, uint32_t n) {
  return Hash32(data, n, 0xDECAFC);
}

inline uint32_t Hash32(const std::string& str) {
  return Hash32(str.data(), static_cast<uint32_t>(str.size()));
}

inline uint64_t Hash64(const char* data, size_t n) {
  return Hash64(data, n, 0xDECAFCAFFE);
}

inline uint64_t Hash64(const std::string& str) {
  return Hash64(str.data(), static_cast<uint64_t>(str.size()));
}

inline uint64_t Hash64Combine(uint64_t a, uint64_t b) {
  return a ^ (b + 0x9e3779b97f4a7800ULL + (a << 10) + (a >> 4));
}

}  // namespace voyager

#endif  // VOYAGER_UTIL_HASH_H_
