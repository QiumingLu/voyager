// Copyright (c) 2016 Mirants Lu. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "voyager/util/hash.h"
#include "voyager/util/coding.h"
#include "voyager/util/macros.h"

namespace voyager {

uint32_t Hash32(const char* data, size_t n, uint32_t seed) {
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.

  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  // Initialize the hash to a 'random' value
  uint32_t h = static_cast<uint32_t>(seed ^ n);

  // Mix 4 bytes at a time into the hash
  while (n >= 4) {
    uint32_t k = DecodeFixed32(data);

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    n -= 4;
  }

  // Handle the last few bytes of the input array

  switch (n) {
    case 3:
      h ^= ByteAsUint32(data[2]) << 16;
      VOYAGER_FALLTHROUGH_INTENDED;
    case 2:
      h ^= ByteAsUint32(data[1]) << 8;
      VOYAGER_FALLTHROUGH_INTENDED;
    case 1:
      h ^= ByteAsUint32(data[0]);
      h *= m;
  }

  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
}

uint64_t Hash64(const char* data, size_t n, uint64_t seed) {
  const uint64_t m = 0xc6a4a7935bd1e995;
  const int r = 47;

  uint64_t h = seed ^ (n * m);

  while (n >= 8) {
    uint64_t k = DecodeFixed64(data);
    data += 8;
    n -= 8;

    k *= m;
    k ^= k >> r;
    k *= m;

    h ^= k;
    h *= m;
  }

  switch (n) {
    case 7:
      h ^= ByteAsUint64(data[6]) << 48;
      VOYAGER_FALLTHROUGH_INTENDED;
    case 6:
      h ^= ByteAsUint64(data[5]) << 40;
      VOYAGER_FALLTHROUGH_INTENDED;
    case 5:
      h ^= ByteAsUint64(data[4]) << 32;
      VOYAGER_FALLTHROUGH_INTENDED;
    case 4:
      h ^= ByteAsUint64(data[3]) << 24;
      VOYAGER_FALLTHROUGH_INTENDED;
    case 3:
      h ^= ByteAsUint64(data[2]) << 16;
      VOYAGER_FALLTHROUGH_INTENDED;
    case 2:
      h ^= ByteAsUint64(data[1]) << 8;
      VOYAGER_FALLTHROUGH_INTENDED;
    case 1:
      h ^= ByteAsUint64(data[0]);
      h *= m;
  }

  h ^= h >> r;
  h *= m;
  h ^= h >> r;

  return h;
}

}  // namespace voyager
