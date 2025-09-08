module;
#include <cstdint>
export module myrocks.env;

import myrocks.common;

namespace MyRocks {

export class SequentialFile {
 public:
  SequentialFile() = default;
  virtual ~SequentialFile() {};
  virtual Status Read(size_t n, Slice* result, char* scratch) = 0;
  virtual Status Skip(uint64_t n) = 0;
};

// A file abstraction for randomly reading the contents of a file.
class RandomAccessFile {
 public:
  RandomAccessFile() = default;
  virtual ~RandomAccessFile() {};

  // Read up to "n" bytes from the file starting at "offset".
  // "scratch[0..n-1]" may be written by this routine.  Sets "*result"
  // to the data that was read (including if fewer than "n" bytes were
  // successfully read).  May set "*result" to point at data in
  // "scratch[0..n-1]", so "scratch[0..n-1]" must be live when
  // "*result" is used.  If an error was encountered, returns a non-OK
  // status.
  //
  // Safe for concurrent use by multiple threads.
  virtual Status Read(uint64_t offset, size_t n, Slice* result,
                      char* scratch) const = 0;
};

// A file abstraction for sequential writing.  The implementation
// must provide buffering since callers may append small fragments
// at a time to the file.
class WritableFile {
 public:
  WritableFile() = default;
  virtual ~WritableFile() {};

  // No copying allowed
  WritableFile(const WritableFile&) = delete;
  WritableFile(const WritableFile&&) = delete;
  WritableFile& operator=(const WritableFile&) = delete;
  WritableFile& operator=(const WritableFile&&) = delete;

  virtual Status Append(const Slice& data) = 0;
  virtual Status Close() = 0;
  virtual Status Flush() = 0;
  virtual Status Sync() = 0;  // sync data

  /*
   * Sync data and/or metadata as well.
   * By default, sync only metadata.
   * Override this method for environments where we need to sync
   * metadata as well.
   */
  virtual Status Fsync() { return Sync(); }

  /*
   * Get the size of valid data in the file.
   */
  virtual uint64_t GetFileSize() { return 0; }
};

class FileLock {
 public:
  FileLock() = default;
  virtual ~FileLock() {};

  FileLock(const FileLock&) = delete;
  FileLock(const FileLock&&) = delete;
  FileLock& operator=(const FileLock&) = delete;
  FileLock& operator=(const FileLock&&) = delete;
};

export class Env {
 public:
  static Env* Default();

  Env() = default;
  virtual ~Env() {};
};
}  // namespace MyRocks
