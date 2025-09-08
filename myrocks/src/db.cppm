module;
#include <string_view>
module myrocks;

namespace MyRocks {
class DBImpl : public MyRocksDB {
 public:
  DBImpl(const Options& options, const std::string_view dbname);
  virtual ~DBImpl();

  DBImpl(const DBImpl&) = delete;
  DBImpl(DBImpl&&) = delete;
  DBImpl& operator=(const DBImpl&) = delete;
  DBImpl& operator=(DBImpl&&) = delete;

 protected:
 private:
};
}  // namespace MyRocks