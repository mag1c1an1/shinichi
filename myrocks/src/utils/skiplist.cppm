module;
#include <atomic>
#include <cassert>
export module myrocks.utils.skiplist;
import myrocks.utils.random;
import myrocks.utils.arena;
namespace MyRocks {
export template <typename K, class C>
class SkipList {
 private:
  struct Node;

 public:
  explicit SkipList(C cmp, Arena* arena);

  SkipList(const SkipList&) = delete;
  SkipList(const SkipList&&) = delete;
  SkipList& operator=(const SkipList&) = delete;
  SkipList& operator=(const SkipList&&) = delete;

  void Insert(const K& key);

  bool Contains(const K& key) const;

  class Iterator {
   public:
    explicit Iterator(const SkipList* list);
    bool Valid() const;
    const K& Key() const;
    void Prev();
    void Next();
    void Seek(const K& target);
    void SeekToFirst();
    void SeekToLast();

   private:
    const SkipList* list_;
    Node* node_;
  };

 private:
  int MaxHeight() const { return max_height_.load(std::memory_order_relaxed); }

  Node* NewNode(const K& key, int height);
  int RandomHeight();
  bool Equal(const K& a, const K& b) const { return (compare_(a, b) == 0); }
  bool KeyIsAfterNode(const K& key, Node* n) const;

  Node* FindGreaterOrEqual(const K& key, Node** prev) const;

  Node* FindLessThan(const K& key) const;

  Node* FindLast() const;

  enum { kMaxHeight = 12 };
  C const compare_;
  Arena* const arena_;
  Node* const head_;
  std::atomic<int> max_height_;
  Node* prev_[kMaxHeight];
  Random rnd_;
};

template <typename Key, class Comparator>
struct SkipList<Key, Comparator>::Node {
  explicit Node(const Key& k) : key(k) {}
  Node* Next(int n) {
    assert(n >= 0);
    return next_[n].load(std::memory_order_acquire);
  }
  void SetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].store(x, std::memory_order_release);
  }

  Node* RelaxedNext(int n) {
    assert(n >= 0);
    return next_[n].load(std::memory_order_relaxed);
  }
  void RelaxedSetNext(int n, Node* x) {
    assert(n >= 0);
    next_[n].store(x, std::memory_order_relaxed);
  }
  Key const key;

 private:
  std::atomic<Node*> next_[1];
};

template <typename K, class C>
typename SkipList<K, C>::Node* SkipList<K, C>::NewNode(const K& key,
                                                       int height) {
  char* mem = arena_->AllocateAligned(
      sizeof(Node) + sizeof(std::atomic<Node*>) * (height - 1));
  return new (mem) Node(key);
}

template <typename K, class C>
SkipList<K, C>::Iterator::Iterator(const SkipList* list) {
  list_ = list;
  node_ = nullptr;
}

template <typename K, class C>
bool SkipList<K, C>::Iterator::Valid() const {
  return node_ != nullptr;
}
template <typename K, class C>
const K& SkipList<K, C>::Iterator::Key() const {
  assert(Valid());
  return node_->key;
}

template <typename K, class C>
void SkipList<K, C>::Iterator::Prev() {
  assert(Valid());
  node_ = list_->FindLessThan(node_->key);
  if (node_ == list_->head_) {
    node_ = nullptr;
  }
}

template <typename K, class C>
void SkipList<K, C>::Iterator::Next() {
  assert(Valid());
  node_ = node_->Next(0);
}

template <typename K, class C>
void SkipList<K, C>::Iterator::Seek(const K& target) {
  node_ = list_->FindGreaterOrEqual(target, nullptr);
}

template <typename K, class C>
void SkipList<K, C>::Iterator::SeekToFirst() {
  node_ = list_->head_->Next(0);
}

template <typename K, class C>
void SkipList<K, C>::Iterator::SeekToLast() {
  node_ = list_->FindLast();
  if (node_ == list_->head_) {
    node_ = nullptr;
  }
}

template <typename K, class C>
int SkipList<K, C>::RandomHeight() {
  // Increase height with probability 1 in kBranching
  static const unsigned int kBranching = 4;
  int height = 1;
  while (height < kMaxHeight && ((rnd_.Next() % kBranching) == 0)) {
    height++;
  }
  assert(height > 0);
  assert(height <= kMaxHeight);
  return height;
}
template <typename K, class C>
bool SkipList<K, C>::KeyIsAfterNode(const K& key, Node* n) const {
  return (n != nullptr) && (compare_(n->key, key) < 0);
}
template <typename K, class C>
typename SkipList<K, C>::Node* SkipList<K, C>::FindGreaterOrEqual(
    const K& key, Node** prev) const {
  if (prev && !KeyIsAfterNode(key, prev[0]->Next(0))) {
    Node* x = prev[0];
    Node* next = x->Next(0);
    if ((x == head_) || KeyIsAfterNode(key, x)) {
      return next;
    }
  }
  Node* x = head_;
  int level = MaxHeight() - 1;
  while (true) {
    Node* next = x->Next(level);
    if (KeyIsAfterNode(key, next)) {
      x = next;
    } else {
      if (prev != nullptr) {
        prev[level] = x;
      }
      if (level == 0) {
        return next;
      } else {
        level--;
      }
    }
  }
}
template <typename K, class C>
typename SkipList<K, C>::Node* SkipList<K, C>::FindLessThan(
    const K& key) const {
  Node* x = head_;
  int level = MaxHeight() - 1;
  while (true) {
    assert(x == head_ || compare_(x->key, key) < 0);
    Node* next = x->Next(level);
    if (next == nullptr || compare_(next->key, key) >= 0) {
      if (level == 0) {
        return x;
      } else {
        // Switch to next list
        level--;
      }
    } else {
      x = next;
    }
  }
}
template <typename K, class C>
typename SkipList<K, C>::Node* SkipList<K, C>::FindLast() const {
  Node* x = head_;
  int level = MaxHeight() - 1;
  while (true) {
    Node* next = x->Next(level);
    if (next == nullptr) {
      if (level == 0) {
        return x;
      } else {
        // Switch to next list
        level--;
      }
    } else {
      x = next;
    }
  }
}

template <typename K, class C>
SkipList<K, C>::SkipList(C cmp, Arena* arena)
    : compare_(cmp),
      arena_(arena),
      head_(NewNode(0, kMaxHeight)),
      max_height_(1),
      rnd_(0xdeadbeef) {
  for (int i = 0; i < kMaxHeight; i++) {
    head_->SetNext(i, nullptr);
    prev_[i] = head_;
  }
}

template <typename K, class C>
void SkipList<K, C>::Insert(const K& key) {
  Node* x = FindGreaterOrEqual(key, prev_);
  assert(x == nullptr || !Equal(key, x->key));
  int height = RandomHeight();
  if (height > MaxHeight()) {
    for (int i = MaxHeight(); i < height; i++) {
      prev_[i] = head_;
    }
    // fprintf(stderr, "Change height from %d to %d\n", max_height_, height);

    // It is ok to mutate max_height_ without any synchronization
    // with concurrent readers.  A concurrent reader that observes
    // the new value of max_height_ will see either the old value of
    // new level pointers from head_ (NULL), or a new value set in
    // the loop below.  In the former case the reader will
    // immediately drop to the next level since NULL sorts after all
    // keys.  In the latter case the reader will use the new node.
    max_height_.store(height, std::memory_order_relaxed);
  }

  x = NewNode(key, height);
  for (int i = 0; i < height; i++) {
    // NoBarrier_SetNext() suffices since we will add a barrier when
    // we publish a pointer to "x" in prev[i].
    x->NoBarrier_SetNext(i, prev_[i]->NoBarrier_Next(i));
    prev_[i]->SetNext(i, x);
  }
  prev_[0] = x;
}

template <typename K, class C>
bool SkipList<K, C>::Contains(const K& key) const {
  Node* x = FindGreaterOrEqual(key, nullptr);
  if (x != nullptr && Equal(key, x->key)) {
    return true;
  } else {
    return false;
  }
}

}  // namespace MyRocks