
#include <cassert>
#include <cstddef>
#include <string>

namespace MyRocks {
class Slice {
public:
  Slice() : data_(""), size_(0) {}

  Slice(const char *d, size_t n) : data_(d), size_(n) {}

  Slice(const std::string &s) : data_(s.data()), size_(s.size()) {}

  Slice(const std::string_view &sv) : data_(sv.data()), size_(sv.size()) {}

  Slice(const char *s) : data_(s) { size_ == (s == nullptr) ? 0 : strlen(s); }

  // create a single slice from SliceParts using buf as storage.
  Slice(const struct SliceParts &ptrs, std::string *buf);

  const char *data() const { return data_; }

  size_t size() const { return size_; }

  bool empty() const { return size_ == 0; }

  char operator[](size_t n) const {
    assert(n < size());
    return data_[n];
  }

  void clear() {
    data_ = "";
    size_ = 0;
  }

  void remove_prefix(size_t n) {
    assert(n <= size());
    data_ += n;
    size_ -= n;
  }

  void remove_suffix(size_t n) {
    assert(n <= size());
    size_ -= n;
  }

  std::string ToString(bool hex = false) const;

  std::string_view ToStringView() const {
    return std::string_view(data_, size_);
  }

  // Decodes the current slice interpreted as an hexadecimal string into result,
  // if successful returns true, if this isn't a valid hex string
  // (e.g not coming from Slice::ToString(true)) DecodeHex returns false.
  // This slice is expected to have an even number of 0-9A-F characters
  // also accepts lowercase (a-f)
  bool DecodeHex(std::string *result) const;

  // Three-way comparison.  Returns value:
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int compare(const Slice &b) const;

  // Return true iff "x" is a prefix of "*this"
  bool starts_with(const Slice &x) const {
    return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
  }

  bool ends_with(const Slice &x) const {
    return ((size_ >= x.size_) &&
            (memcmp(data_ + size_ - x.size_, x.data_, x.size_) == 0));
  }

  // Compare two slices and returns the first byte where they differ
  size_t difference_offset(const Slice &b) const;

private:
  const char *data_;
  size_t size_;
};
} // namespace MyRocks
