module;
#include <cassert>
#include <cstddef>
#include <cstring>
#include <string>

module myrocks.common:slice;

namespace MyRocks {

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
std::string Slice::ToString(bool hex = false) const {
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
bool Slice::StartWith(const Slice& x) const {
  return ((size_ >= x.size_) && (memcmp(data_, x.data_, x.size_) == 0));
}
char Slice::operator[](size_t n) const {
  assert(n < Size());
  return data_[n];
}
bool Slice::operator==(const Slice& other) const {
    return (Size() == other.Size() )&&( memcmp(Data(),other.Data(),Size()) == 0);
}

bool Slice::operator!=(const Slice& other) const {
    return !(*this == other);
}

}  // namespace MyRocks