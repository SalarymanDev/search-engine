#ifndef INDEXER_H_
#define INDEXER_H_

#include "src/libs/index/PostingList/PostingList.h"
#include "src/libs/index/rankingData/rankingData.h"
#include "src/libs/index/Blob/Blob.h"
#include "src/libs/index/HashTable/HashTable.h"
#include "src/libs/preprocessedDocument/PreprocessedDocument.h"
#include "src/libs/map/map.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/index/index.h"

#include "src/libs/utils/utils.h"
#include "src/libs/preprocessing/loaders/loaders.h"
#include "src/libs/logger/logger.h"
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <cmath>

namespace BBG { 
    
    class Indexer {

    private:
        friend class IndexChunk;

        vector<uint32_t> wordIDs;

        uint32_t numUniqueWords;
        Map<uint32_t, uint32_t> wordCountMap;

        uint32_t numTotalWords;
        uint32_t numDocuments;

        HashTable< uint32_t, size_t > postingsHash;
        // leave 0 for seek no return
        uint32_t currentPostLocation; 
        
        // effectively a set: words to all documents using this word
        Map< uint32_t, Map< uint32_t, bool > > docIds;

        vector< EndDocPost > endDocPosts;
 
        Logger logger;

        Map<string, uint32_t>& BagOfWords;
        Map<uint32_t, Map<string, uint32_t>>& tf; 
        Map<uint32_t, float>& idf;
        Map<string, float>& NORM;

    public:
    // TODO: Establish best way to get this data with ranking crew. 
    //      Thrown here just for testing, but should load from a file.
        // Map< uint32_t, WordFeatures > wordFeatures;
        Map< uint32_t, vector<Post> > posts;

        Indexer(Map<string, uint32_t>& _bow, Map<uint32_t, Map<string, uint32_t>>& _tf, 
                Map<uint32_t, float>& _idf, Map<string, float>& _NORM);

        void createIndexChunk( const char* indexFile, vector<PreprocessedDocument>& docs );

        // helper functions only exposed for testing
        void writeIndexChunk( IndexChunk* index, vector<PreprocessedDocument>& docs);
        size_t collectFeatures( vector< PreprocessedDocument > & doc );
        size_t collectFeatureFromVector( vector< string > & words, size_t docNumber, HtmlTag HtmlTag);
        // For testing. Uses BagOfWords to convert word to Id
        uint32_t wordToID(const string& word);


        static void extractUrlWords( string& url, vector< string >& vec );
    };

}

#endif