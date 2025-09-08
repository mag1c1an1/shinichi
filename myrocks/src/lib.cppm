module;

#include <string>
#include <string_view>
#include <cstdint>
#include <vector>

export module myrocks;

export import myrocks.common;
export import myrocks.iterator;

namespace MyRocks {

// Abstract handle to particular state of a DB.
// A Snapshot is an immutable object and can therefore be safely
// accessed from multiple threads without any external synchronization.
export class SnapShot {
 protected:
  virtual ~SnapShot();
};

export struct Range {
  Range() {}
  Range(const Slice& s, const Slice& l) : start(s), limit(l) {}
  Slice start;  // include
  Slice limit;  // not include
};

export class MyRocksDB {
 public:
  static Status Open(const Options& options, std::string_view name,
                     MyRocksDB** dbptr);
  static Status OpenForReadOnly(const Options& options, std::string_view name,
                                MyRocksDB** dbptr,
                                bool error_if_log_file_exist = false);
  MyRocksDB() {}
  virtual ~MyRocksDB();
  // No copying allowed
  MyRocksDB(const MyRocksDB&) = delete;
  void operator=(const MyRocksDB&&) = delete;

  virtual Status Put(const WriteOptions& options, const Slice& key,
                     const Slice& value) = 0;
  virtual Status Delete(const WriteOptions& options, const Slice& key) = 0;
  virtual Status Get(const ReadOptions& options, const Slice& Key,
                     std::string* value) = 0;
  virtual Iterator* NewIterator(const ReadOptions& options) = 0;
  virtual const SnapShot* GetSnapshot() = 0;
  virtual void ReleaseSnapshot(const SnapShot* snapshot) = 0;
  virtual bool GetProperty(const Slice& property, std::string* value) = 0;
  virtual void GetApproximateSizes(const Range* range, int n,
                                   uint64_t* sizes) = 0;
  virtual void CompactRange(const Slice* begin, const Slice* end) = 0;
  virtual int NumberLevels() = 0;
  virtual int MaxMemCompactionLevel() = 0;
  virtual int Level0StopWriteTrigger() = 0;
  virtual Status Flush(const FlushOptions& options) = 0;
  virtual Status EnableFileDeletions() = 0;
  virtual Status DisableFileDeletions() = 0;
  virtual Status GetLiveFiles(std::vector<std::string>&,
                              uint64_t* manifest_file_size) = 0;
  virtual SequenceNumber GetLatestSequenceNumber() = 0;
  virtual Status GetUpdatesSince(SequenceNumber seq_number,
                                 TransactionLogIterator** iter) = 0;
};

export Status DestoryDB(std::string_view name, const Options& options);

export Status RepairDB(std::string_view name, const Options& options);

// impl

}  // namespace MyRocks
