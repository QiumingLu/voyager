#ifndef EXAMPLES_CACHE_HASH_H_
#define EXAMPLES_CACHE_HASH_H_

#include <stddef.h>
#include <stdint.h>

namespace voyager {
namespace cache {

extern uint32_t Hash(const char* data, size_t n, size_t seed);

}  // namespace cache
}  // namespace voyager

#endif  // EXAMPLES_CACHE_HASH_H_
