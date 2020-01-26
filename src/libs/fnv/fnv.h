#ifndef FNV64_H
#define FNV64_H

#include <cstdint>
#include "src/libs/string/string.h"

namespace BBG {
  static uint64_t fnv(const void *data, int len) {
    // Magic offset and prime. See
    // http://isthe.com/chongo/tech/comp/fnv/#FNV-param
    uint64_t _hash = 0xCBF29CE484222325;
    uint64_t _prime = 0x100000001B3;

    for (int i = 0; i < len; i++) {
      _hash ^= ((char *)data)[i];
      _hash *= _prime;
    }
    return _hash;
  }

  static inline uint64_t fnv(const string *s, int len) {
    return fnv(s->c_str(), s->size());
  }
}
#endif
