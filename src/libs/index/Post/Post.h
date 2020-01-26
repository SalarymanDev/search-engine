#ifndef POST_H_
#define POST_H_

#include "src/libs/index/rankingData/rankingData.h"
#include "src/libs/string/string.h"
#include "src/libs/index/LEB128/LEB128.h"

namespace BBG 
    { 

    class Post 
        { 
        public:
        static const uint32_t HtmlShift = 30;
        static const uint32_t HtmlMask = 0xC0000000;
        static const uint32_t LocationMask = 0x3FFFFFFF;

        //         Bits  |   31-30   |   29-0   |
        // attributes := | Html tag  |  absLoc  |
        uint32_t location;

        Post( );

        Post( uint32_t _location, uint32_t tag );
        
        
        HtmlTag getHtmlTag( ) const;
        void setTag( uint32_t tag );
        uint32_t getAbsoluteLocation( ) const;

        void setLocation( uint32_t _location );
        uint32_t getTermFrequency( uint32_t docEnd) ;

        Post& operator=( const Post& other );

        bool operator==( const Post& other ) const;

        char * encode( char* buf ) const;
        const char * decode( const char * buf );
        size_t byteSize( ) const;
        };

    class EndDocPost 
        {
        public:
        DocFeatures docFeatures;
        size_t bytesToUrl;
        // size_t bytesToTitle;
        uint32_t location;
        uint32_t docId;

        // url follows immediately

        EndDocPost( );
        EndDocPost( uint32_t _location, uint32_t doc );
        EndDocPost( uint32_t _location, uint32_t doc, DocFeatures _features );
        
        const string getUrl( ) const;
        const string getTitle( ) const;

        uint32_t getAbsoluteLocation( ) const;
        void setLocation( uint32_t _location );
        // TODO: add getFeatures() or just accesss through ptr?

        EndDocPost& operator=( const EndDocPost& other );

        bool operator==( const EndDocPost& other ) const;

        uint32_t docStart( ) const;

        char * encode( char* buf ) const;
        const char * decode( const char * buf );
        size_t byteSize( ) const;
        };
    }

#endif