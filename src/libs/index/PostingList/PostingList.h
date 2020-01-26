#ifndef POSTING_LIST_H_
#define POSTING_LIST_H_

#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/utils/utils.h"
#include "src/libs/index/rankingData/rankingData.h"
#include "src/libs/preprocessedDocument/PreprocessedDocument.h"

#include "src/libs/index/Post/Post.h"
namespace BBG 
    {
    class SeekTableEntry 
        {
        public:
        /**************  Uncomment for SeekTable Size of 4096 ********************* */
        // 12 high bits corresponds to seek table of size 2^12 = 4k ish 
        static const uint32_t HighBits = 0x3FFC0000;
        static const uint32_t TableSize = 4096;

        // numLowBits corresponds to number of 0s in HighBit mask
        static const uint32_t numLowBits = 18;
        
         /**************  Uncomment for SeekTable Size of 8192 ********************/
        // 13 high bits corresponds to seek table of size 2^13 = 8k ish 
        // static const size_t HighBits = 0x3FFE0000;
        // static const size_t TableSize = 8192;

        // // numLowBits corresponds to number of 0s in HighBit mask
        // static const uint32_t numLowBits = 17;
        

        /**************  Uncomment for SeekTable Size of 16384 ********************/
        // 14 high bits corresponds to seek table of size 2^14 = 16k ish
        // static const uint32_t HighBits = 0x3FFF0000;
        // static const uint32_t TableSize = 16384;
        
        // // numLowBits corresponds to number of 0s in HighBit mask
        // static const uint32_t numLowBits = 16;
        
        uint32_t PostingListPosition, AbsolutePosition;
        };

    class PostingList 
        {
        public:
        //    CommonHeader  
        WordFeatures wordFeatures; 
        // bytesToPostsEnd is also first byte past all posts
        size_t bytesToPostsEnd;
        // uint32_t numberOfPosts;    

        // Immediately following header
        // Fixed size seektable
        // then Post list
        // *** bytesToFeatures points here ***
        // then WordDocFeatures map

        // returns offset to end of postingList
        static uint32_t WriteSeekTable(  PostingList * pList, vector<Post>& locations );
        static uint32_t WriteSeekTable(  PostingList * pList, vector<EndDocPost>& locations );
        
        static uint32_t addPosts(PostingList * pList, uint32_t postOffset, vector<Post> & posts);
        
        static void addFeature(PostingList *pList, uint32_t docId, WordDocFeatures& feature );
       
        static uint32_t addEndPostsAndUrls( PostingList *pList, uint32_t postOffset, 
                vector<EndDocPost>& endPosts, vector< PreprocessedDocument > & docs );

        uint32_t getPostLoc ( size_t target );

        static size_t postBytesRequired( size_t numPosts, size_t numDocs );
        static size_t endDocBytesRequired( vector< PreprocessedDocument >& docs );
        
        WordDocFeatures * getWordDocFeature( const EndDocPost* endDoc );

        Post * Seek( size_t target );
        EndDocPost * SeekEndDoc( size_t target );
        char * getSentinel( );
        // TODO: implement/ correct these after feature data
        uint32_t getByteLength( );
        Post * getLastPost( );
        EndDocPost * getLastEndDocPost( );
        };
    }

#endif
