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
  void RemovePrefix(size_t n) ;

  // Return a string that contains the copy of the referenced data.
  std::string ToString(bool hex = false) const ;

  // Three-way comparison.  Returns value:
  //   <  0 iff "*this" <  "b",
  //   == 0 iff "*this" == "b",
  //   >  0 iff "*this" >  "b"
  int Compare(const Slice& b) const;

  // Return true iff "x" is a prefix of "*this"
  bool StartsWith(const Slice& x) const ;


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

}  // namespace MyRocks