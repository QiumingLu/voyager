#ifndef EXAMPLES_CACHE_SKIPLIST_H_
#define EXAMPLES_CACHE_SKIPLIST_H_

#include <atomic>

#include "examples/cache/arena.h"
#include "examples/cache/random.h"

namespace voyager {
namespace cache {

template <typename Key, class Comparator>
class SkipList {
 private:
  struct Node;

 public:
  explicit SkipList(Comparator cmp, Arena* arena);

  void Insert(const Key& key);

  bool Contains(const Key& key) const;

 private:
  enum { kMaxHeight = 12 };

  Comparator const compare_;
  Arena* const arena_;

  Node* const head_;

  std::atomic<int> max_height_;

  inline int GetMaxHeight() const {
    return max_height_.load(std::memory_order_relaxed);
  }

  // Read/written only by Insert()
  Random rnd_; 
 
  Node* NewNode(const Key& key, int height);
  int RandomHeight();
  bool Equal(const Key& a, const Key& b) const { 
    return (compare_(a, b) == 0); 
  }

  // Return true if key is greater than the data stored in "n"
  bool KeyIsAfterNode(const Key& key, Node* n) const;

  Node* FindGreaterOrEqual(const Key& key, Node** prev) const;

  Node* FindLessThan(const Key& key) const;

  Node* FindLast() const;

  // No copying allow
  SkipList(const SkipList&);
  void operator=(const SkipList&);
};

template <typename Key, class Comparator>
struct SkipList<Key, Comparator>::Node {
  explicit Node(const Key& k) : key(k) { }

  Key const key;

  Node* Next(int n) {
    assert(n >= 0);
    // Use an 'acquire load' so that we observe a fully initialized
    // version of the returned Node.
    return next_[n].load(std::memory_order_acquire);
  }
  void SetNext(int n, Node* x) {
    assert(n >= 0);
    // Use a 'release store' so that anybody who reads through this
    // poniter observes a fully initialized version.
    next_[n].store(x, std::memory_order_release);
  }

  // No-barrier variants that can be safely used in a few locations.
  Node* NoBarrier_Next(int n) {
    assert(n >= 0);
    return next_[n].load(std::memory_order_relaxed);
  }
  void NoBarrier_SetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].store(x, std::memory_order_relaxed);
  }

 private:
  std::atomic<Node*> next_[1];
};

template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node* 
SkipList<Key, Comparator>::NewNode(const Key& key, int high) {
  char* mem = arena_->AllocateAligned(
      sizeof(Node) + sizeof(std::atomic<Node*>) * (max_height_ - 1));
  return new (mem) Node(key);
}

template <typename Key, class Comparator>
int SkipList<Key, Comparator>::RandomHeight() {
  // Increase height with probility 1 in kBranching.
  static const unsigned int kBranching = 4;
  int height = 1;
  while (height < kMaxHeight && ((rnd_.Next() % kBranching) == 0)) {
    height++;
  }
  assert(height > 0);
  assert(height <= kMaxHeight);
  return height;
}

template <typename Key, class Comparator>
bool SkipList<Key, Comparator>::KeyIsAfterNode(const Key& key, Node* n) const {
  // NULL n is considered infinite
  return (n != NULL) && (compare_(n->key, key) < 0);
}

template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node* 
SkipList<Key, Comparator>::FindGreaterOrEqual(const Key& key, 
                                             Node** prev) const{
  Node* x = head_;
  return x;
}

template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node* 
SkipList<Key, Comparator>::FindLessThan(const Key& key) const {
  Node* x = head_;
  return x;
}

template <typename Key, class Comparator>
typename SkipList<Key, Comparator>::Node* 
SkipList<Key, Comparator>::FindLast() const {
  Node* x = head_;
  return x;
}

template <typename Key, class Comparator>
SkipList<Key, Comparator>::SkipList(Comparator cmp, Arena* arena)
    : compare_(cmp),
      arena_(arena),
      head_(NewNode(0, kMaxHeight)),
      rnd_(0xdeadbeef) {
  for (int i = 0; i < kMaxHeight; ++i) {
    head_->SetNext(i, NULL);
  }
}

template <typename Key, class Comparator>
void SkipList<Key, Comparator>::Insert(const Key& key) {
}

template <typename Key, class Comparator>
bool SkipList<Key, Comparator>::Contains(const Key& key) const {
  return true;
}

}  // namespace cache
}  // namespace voyager

#endif  // EXAMPLES_CACHE_SKIPLIST_H_
