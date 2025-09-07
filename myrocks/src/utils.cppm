module;
#include <cstdint>

export module myrocks.utils;
export import myrocks.utils.arena;

namespace MyRocks{
export class Random{
public:
  explicit Random(uint32_t s); 
  uint32_t Next();
  // Returns a uniformly distributed value in the range [0..n-1]
  // REQUIRES: n > 0
  uint32_t Uniform(int n);

  // Randomly returns true ~"1/n" of the time, and false otherwise.
  // REQUIRES: n > 0
  bool OneIn(int n);

  // Skewed: pick "base" uniformly from range [0,max_log] and then
  // return "base" random bits.  The effect is to pick a number in the
  // range [0,2^max_log-1] with exponential bias towards smaller numbers.
  uint32_t Skewed(int max_log);
  
private:
  uint32_t seed_;
};
}

