module;

#incldue <string>
#incldue <string_view>

export module myrocks;

export import myrocks.common;
export import myrocks.iterator;

namespace MyRocks {

class SnapShot {
 protected:
};

struct Range {
  Range() {}
  Range(const Slice& s, const Slice& l) : start(s), limit(l) {}
  Slice start; // include
  Slice limit; // not include
};

export class MyRocksDB {
 public:
  static Status Open(const Options& options,std::string_view name,DB** dbptr);
  Status Put();

  void Print();

  MyRocksDB() {}
  // No copying allowed
  MyRocksDB(const MyRocksDB&) = delete;
  void operator=(const MyRocksDB&&) = delete;
  virtual ~MyRocksDB() {}
};

export DestoryDB(std::string_view name, const Options& options);

export RepairDB(std::string_view name, const Options& options);


}  // namespace MyRocks
