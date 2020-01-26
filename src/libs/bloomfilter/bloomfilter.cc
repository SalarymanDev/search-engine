#include "src/libs/bloomfilter/bloomfilter.h"

void BBG::serialize(const Bloomfilter &filter, Buffer &bytes) {
    serialize(filter._num_hashes, bytes);
    serialize(filter._count, bytes);
    serialize(filter._bitset, bytes);
}

void BBG::deserialize(Buffer &bytes, Bloomfilter &filter) {
    deserialize(bytes, filter._num_hashes);
    deserialize(bytes, filter._count);
    deserialize(bytes, filter._bitset);
}
