#ifndef INDEX_H_
#define INDEX_H_

#include "src/libs/index/PostingList/PostingList.h"
#include "src/libs/map/map.h"
#include "src/libs/index/Blob/Blob.h"
#include "src/libs/logger/logger.h"
#include "src/libs/utils/utils.h"

namespace BBG {
  class IndexChunk {

  public:
    friend class Indexer;
    // Map<string, uint32_t> BagOfWords;
    // Indexer should create
    // static IndexChunk* create( IndexChunk* index, uint32_t* buffer, size_t bytes );
    
    uint32_t WordsInChunk, DocumentsInChunk, LocationsInChunk, MaximumLocation;
    //
    //
    // Blob * dictionary will follow in memory immediately

    PostingList * getPostingList(const uint32_t word);
    PostingList * getEndDocPostingList();

    uint32_t GetNumberOfWords( );
    //  uint32_t GetNumberOfUniqueWords( );
    uint32_t GetNumberOfDocuments( );

  };

  class IndexFileReader {

  private:
    int fd;
    size_t fileSize;
    IndexChunk * chunk;
    Logger logger;

  public:
    IndexFileReader(const char* filename);
    ~IndexFileReader( );
    IndexChunk * Load() const;
  };
}

#endif
