#ifndef EXAMPLES_CACHE_RANDOM_H_
#define EXAMPLES_CACHE_RANDOM_H_

namespace voyager {
namespace cache {

class Random {
 private:
  uint32_t seed_;

 public:
  explicit Random(uint32_t s) : seed_(s & 0x7fffffffu) {
    // Avoid bad seeds.
    if (seed_ == 0 || seed_ == 2147483647L) {
      seed_ = 1;
    }
  }

  uint32_t Next() {
    return seed_;
  }
};

}  // namespace cache
}  // namespace voyager

#endif  // EXAMPLES_CACHE_RANDOM_H_
