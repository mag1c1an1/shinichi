module myrocks.env;

namespace MyRocks {

namespace {

class PosixSequentialFile : public SequentialFile {
 private:
  std::string filename_;
  File* file_;
};

}  // namespace

}  // namespace MyRocks