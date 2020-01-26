#ifndef SEEN_H
#define SEEN_H


#include "src/libs/bloomfilter/bloomfilter.h"
#include "src/libs/vector/vector.h"


namespace BBG {

    class SeenSet {
    private:
      Bloomfilter processing;
      Bloomfilter visited;
      
    public:

      SeenSet() {}
      SeenSet(float falsePositiveRate, int expectedElements);
      SeenSet(vector<bool> bitSet, int numHashes);
      
      // For checking whether or not url has been visted or being processed 
      bool seenUrl(const string& url);
      void markUrlProcessing(const string& url);
      void markUrlVisited(const string& url);

      // For backing up to disk
      const vector<bool>& getVisitedBitSet();
      const int getNumHashes();

      friend void serialize(const SeenSet &seen, Buffer &bytes);
      friend void deserialize(Buffer &bytes, SeenSet &seen);

    };

    void serialize(const SeenSet &seen, Buffer &bytes);
    void deserialize(Buffer &bytes, SeenSet &seen);

}

#endif
