module;
#include <memory>


export module myrocks.common:status;
import :slice;

namespace MyRocks {
export class Status {
 public:
  Status();
  ~Status();

  Status(const Status& s);
  Status& operator=(const Status& s);
  Status(Status&& s) noexcept;
  Status& operator=(Status&& s) noexcept;
  bool operator==(const Status& rhs) const;
  bool operator!=(const Status& rhs) const;

  enum Code : unsigned char {
    kOk = 0,
  };

  //   Code code() const {

  //   }
  enum SubCode : unsigned char {
    kNone = 0,
  };

  enum Severity : unsigned char {
    kNoError = 0,
  };

 protected:
  std::unique_ptr<const char[]> state_;
};
}  // namespace MyRocks
