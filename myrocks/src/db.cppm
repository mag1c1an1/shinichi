module myrocks;

namespace MyRocks {
class DBImpl : public MyRocksDB {
 public:
  DBImpl(const DBImpl&) = delete;
  DBImpl(DBImpl&&) = delete;
  DBImpl& operator=(const DBImpl&) = delete;
  DBImpl& operator=(DBImpl&&) = delete;
};
}  // namespace MyRocks