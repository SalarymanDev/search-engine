#include "src/libs/index/HashTable/HashTable.h"

bool BBG::CompareEqual(uint32_t L, uint32_t R) {
   return L == R;
}

bool BBG::CompareEqual(const char* L, const char* R) {
   return BBG::strcmp(L, R);
}

uint32_t BBG::fnvHash(const char* data, size_t length) {
    static const size_t FnvOffsetBasis = 146959810393466560;
    static const size_t FnvPrime = 1099511628211ul;
    size_t hash = FnvOffsetBasis;
    for( size_t i = 0; i < length; ++i ) {
        hash *= FnvPrime;
        hash ^= data[ i ];
    }
    return hash;
}
