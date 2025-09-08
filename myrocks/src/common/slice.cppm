module;
#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>
export module myrocks.common:slice;

namespace MyRocks {
export class Slice {
 public:
  // Create an empty slice.
  Slice();

  // Create a slice that refers to d[0,n-1].
  Slice(const char* d, size_t n);

  // Create a slice that refers to the contents of "s"
  Slice(const std::string& s);

  // Create a slice that refers to s[0,strlen(s)-1]
  Slice(const char* s);

  // Return a pointer to the beginning of the referenced data
  const char* Data() const { return data_; }

  // Return the length (in bytes) of the referenced data
  size_t Size() const { return size_; }

  // Return true iff the length of the referenced data is zero
  bool IsEmpty() const { return size_ == 0; }

  // Change this slice to refer to an empty array
  void Clear();

  // Drop the first "n" bytes from this slice.
  void RemovePrefix(size_t n);

  // Return a string that contains the copy of the referenced data.
  std::string ToString(bool hex = false) const;

  // Three-way comparison.  Returns value:
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int Compare(const Slice& b) const;

  // Return true iff "x" is a prefix of "*this"
  bool StartsWith(const Slice& x) const;

  // Return the ith byte in the referenced data.
  // REQUIRES: n < size()
  char operator[](size_t n) const;

  bool operator==(const Slice& other) const;

  bool operator!=(const Slice& other) const;

  // make these public for leveldbjni access
 private:
  const char* data_;
  size_t size_;
};
Slice::Slice() : data_(""), size_(0) {}

Slice::Slice(const char* d, size_t n) : data_(d), size_(n) {}

Slice::Slice(const std::string& s) : data_(s.data()), size_(s.size()) {}

Slice::Slice(const char* s) : data_(s), size_(strlen(s)) {}

void Slice::Clear() {
  data_ = "";
  size_ = 0;
}

void Slice::RemovePrefix(size_t n) {
  assert(n <= Size());
  data_ += n;
  size_ -= n;
}

// Return a string that contains the copy of the referenced data.
std::string Slice::ToString(bool hex) const {
  if (hex) {
    std::string result;
    char buf[10];
    for (size_t i = 0; i < size_; i++) {
      snprintf(buf, 10, "%02X", (unsigned char)data_[i]);
      result += buf;
    }
    return result;
  } else {
    return std::string(data_, size_);
  }
}

// Three-way comparison.  Returns value:
//   <  0 iff "*this" <  "b",
//   == 0 iff "*this" == "b",
//   >  0 iff "*this" >  "b"
int Slice::Compare(const Slice& b) const {
  const int min_len = (size_ < b.size_) ? size_ : b.size_;
  int r = memcmp(data_, b.data_, min_len);
  if (r == 0) {
    if (size_ < b.size_)
      r = -1;
    else if (size_ > b.size_)
      r = +1;
  }
  return r;
}

// Return true iff "x" is a prefix of "*this"
bool Slice::StartsWith(const Slice& x) const {
  return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
}
char Slice::operator[](size_t n) const {
  assert(n < Size());
  return data_[n];
}
bool Slice::operator==(const Slice& other) const {
  return (Size() == other.Size()) &&
         (memcmp(Data(), other.Data(), Size()) == 0);
}

bool Slice::operator!=(const Slice& other) const { return !(*this == other); }
}  // namespace MyRocks