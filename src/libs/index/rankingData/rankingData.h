#ifndef RANKING_DATA_H_
#define RANKING_DATA_H_

#include <stdint.h> // uint32_t
#include "src/libs/index/LEB128/LEB128.h"

using namespace BBG;

namespace BBG 
    {

// This struct contains all features of a UNIQUE document
    struct DocFeatures 
        {
    // *** Unsiged Integer types ***//
        uint32_t numWords;

        //         Bits  |   31-16   |     15-0      |
        // attributes := | imgCount  |  titleLength  |
        uint32_t imgCountTitleLength;

        static const uint32_t titleLengthMask = 0xFFFF;
        static const uint32_t imgCountShift = 16;
        uint32_t getTitleLength( );
        uint32_t getImgCount( );

        //         Bits  |   31-16   |    15-0    |
        // attributes := | incoming  |  outgoing  |
        uint32_t linkCounts;

        static const uint32_t outgoingLinkMask = 0xFFFF;
        static const uint32_t incomingLinkShift = 16;
        uint32_t getIncomingLinkCount( );
        uint32_t getOutgoingLinkCount( );

    // *** floats and doubles ***//
        float docNorm;

    // *** NO OTHER TYPES SUPPORT ***//
        // char[]  and strings need special storage in Post and PostingList
        
        // I don't know best way to construct these. Up to rankers discretion
        DocFeatures( );
        DocFeatures( uint32_t _numWords, uint32_t _titleLength, uint32_t _imgCount, 
            size_t _incomingLinkCount, size_t _outgoingLinkCount, float _docNorm );
        DocFeatures( const DocFeatures& other );
        
        DocFeatures& operator=( const DocFeatures& other );

        bool operator==( const DocFeatures& other ) const;

        // endcodes all member variables into buf
        // returns pointer just past last written byte in buf
        // VITAL: encoding order of member variables must match decoding order!
        char * encode( char* buf ) const;
        const char * decode( const char * buf );
        size_t byteSize( ) const;
        };

// This struct contains all features of a UNIQUE word
    struct WordFeatures 
        {
    // *** Unsiged Integer types ***//
        
    // *** floats and doubles ***//
        float idf; 

    // *** NO OTHER TYPES SUPPORT ***//
        // char[]  and strings need special storage in Post and PostingList

        // I don't know best way to construct these. Up to rankers discretion
        WordFeatures( );
        WordFeatures( float _idf );

        WordFeatures( const WordFeatures& other );

        WordFeatures& operator=( const WordFeatures& other );

        bool operator==( const WordFeatures& other ) const;

        // VITAL: encoding order of member variables must match decoding order!
        // endcodes all member variables into buf
        // returns pointer just past last written byte in buf
        char * encode( char* buf ) const;
        const char * decode( const char * buf );
        size_t byteSize( );
        };

// This struct contains all features of a UNIQUE word in a particular document
    struct WordDocFeatures 
        {

    // *** Unsiged Integer types ***//

        //         Bits  |   31-16   |     15-0      |
        // attributes := | termFreq  |  anchorCount  |
        uint32_t termFreqAnchorCount;
        static const uint32_t anchorCountMask = 0xFFFF;
        static const uint32_t termFreqShift = 16;

        uint32_t getTermFreq( );
        // counts number of documents linking to this doc using this term
        uint32_t getAnchorTermCount( );
    // *** floats and doubles ***//

    // *** NO OTHER TYPES SUPPORT ***//
        // char[]  and strings need special storage in Post and PostingList

        // I don't know best way to construct these. Up to rankers discretion
        WordDocFeatures( );
        WordDocFeatures( uint32_t _anchorTermCount, uint32_t _termFreq );

        WordDocFeatures( const WordDocFeatures& other );

        WordDocFeatures& operator=( const WordDocFeatures& other );

        bool operator==( const WordDocFeatures& other ) const;

    // VITAL: encoding order of member variables must match decoding order!

        // endcodes all member variables into buf
        // returns pointer just past last written byte in buf
        char * encode( char* buf ) const;

        const char * decode( const char * buf );
        };

    // NOTE: features for a word instance must go in posting list. 
    // Examples: word's html type like body/title/anchor.
    enum HtmlTag 
        { 
        Body = 0,
        Title = 1,
        Url = 2,
        Anchor = 3
        };
    }
#endif