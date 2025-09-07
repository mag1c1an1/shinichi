module;

export module myrocks.iterator;

import myrocks.common;

namespace MyRocks {
class Iterator {
 public:
  Iterator();
  ~Iterator();

  Slice key() const;
};
}  // namespace MyRocks
