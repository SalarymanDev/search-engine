#ifndef BLOOMFILTER_H
#define BLOOMFILTER_H
#include <cmath>

#include "src/libs/fnv/fnv.h"
#include "src/libs/vector/vector.h"
#include "src/libs/serialize/Buffer.h"

namespace BBG {
  class Bloomfilter {
   public:
    /**
     * @param false_positive_rate the acceptable rate of false positives when
     * testing for set membership
     * @param expected_elements the number of unique elements expected to be in
     * the bloom filter
     */
    Bloomfilter() {}

    Bloomfilter(float false_positive_rate, uint64_t expected_elements) {
      bloomfilterImpl(false_positive_rate, expected_elements);
    };

    Bloomfilter(vector<bool> bitset, uint64_t num_hashes) :
      _num_hashes(num_hashes), _bitset(bitset) {}

    void insert(const void *data, uint64_t len) {
      uint64_t hash = fnv(data, len);
      for (uint64_t k = 0; k < _num_hashes; k++) {
        uint64_t new_hash = (hash & 0xFFFF0000) + (k * hash & 0x0000FFFF);
        _bitset[new_hash % (_bitset.size() - 1)] = true;
      }
      _count++;
    };

    bool contains(const void *data, uint64_t len) {
      uint64_t hash = fnv(data, len);
      for (uint64_t k = 0; k < _num_hashes; k++) {
        uint64_t new_hash = (hash & 0xFFFF0000) + (k * hash & 0x0000FFFF);
        if (_bitset[new_hash % (_bitset.size() - 1)] == false) {
          return false;
        }
      }
      return true;
    };

    const vector<bool>& getBitSet() {
      return _bitset;
    }

    const uint64_t getNumHashes() {
      return _num_hashes;
    }

    const size_t count(){
      return _count;
    }

   private:
    uint64_t _num_hashes;
    vector<bool> _bitset;
    uint64_t _count;

    // Calculate needed hash functions and allocate memory
    void bloomfilterImpl(float false_positive_rate, uint64_t expected_elements) {
      // Calculate bitvector size
      uint64_t m = -(expected_elements * log(false_positive_rate)) / pow(log(2), 2);
      _bitset.resize(m);
      // Calcualte needed hash functions
      _num_hashes = (m / expected_elements) * log(2);
    }

    friend void serialize(const Bloomfilter &filter, Buffer &bytes);
    friend void deserialize(Buffer &bytes, Bloomfilter &filter);
  };

  void serialize(const Bloomfilter &filter, Buffer &bytes);

  void deserialize(Buffer &bytes, Bloomfilter &filter);

}
#endif
