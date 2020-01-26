#include "src/libs/frontier/seen/seen.h"

using BBG::SeenSet;
using BBG::vector;


SeenSet::SeenSet(float falsePositiveRate, int expectedElements) 
    : processing(falsePositiveRate, expectedElements),
      visited(falsePositiveRate, expectedElements) {}

SeenSet::SeenSet(vector<bool> bitset, int numHashes)
    : processing(bitset, numHashes),
      visited(bitset, numHashes) {}

bool SeenSet::seenUrl(const string& url) {
   return processing.contains(url.c_str(), url.size()) ||
          visited.contains(url.c_str(), url.size());
}

void SeenSet::markUrlProcessing(const string& url) {
   processing.insert(url.c_str(), url.size());
}

void SeenSet::markUrlVisited(const string& url) {
   visited.insert(url.c_str(), url.size());
}

const vector<bool>& SeenSet::getVisitedBitSet() {
   return visited.getBitSet();
}

void BBG::serialize(const SeenSet &seen, Buffer &bytes) {
   serialize(seen.visited, bytes);
}

void BBG::deserialize(Buffer &bytes, SeenSet &seen) {
   deserialize(bytes, seen.visited);
   seen.processing = Bloomfilter(vector<bool>(seen.visited.getBitSet().size()), seen.visited.getNumHashes());
}
