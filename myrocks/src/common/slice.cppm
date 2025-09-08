module;
#include <cstddef>

export module myrocks.common:slice;

namespace MyRocks {
export class Slice {
 public:
  Slice() : data_(""), size_(0) {}

  inline bool is_empty() const { return size_ == 0; }

 private:
  const char* data_;
  size_t size_;
};
}  // namespace MyRocks