module;

export module myrocks.iterator;

import myrocks.common;

namespace MyRocks {
export class Iterator {
 public:
  Iterator();
  ~Iterator();

  Slice key() const;
};

export class TransactionLogIterator {};

}  // namespace MyRocks
