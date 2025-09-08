module;
#include <cstdint>
export module myrocks.utils.murmurhash;

export uint64_t MurmurHash64A(const void* key, int len, unsigned int seed);