
#include "src/libs/index/rankingData/rankingData.h"

using namespace BBG;

/////// DocFeatures

DocFeatures::DocFeatures( ): numWords( 0 ), imgCountTitleLength( 0 ), 
        linkCounts( 0 ), docNorm( 0.0f ) { }


DocFeatures::DocFeatures( uint32_t _numWords, uint32_t _titleLength, uint32_t _imgCount, 
            size_t _incomingLinkCount, size_t _outgoingLinkCount, float _docNorm ) : 
    numWords( _numWords ), docNorm( _docNorm ) 
    {
    linkCounts = ( _incomingLinkCount << incomingLinkShift ) 
                    + ( _outgoingLinkCount & outgoingLinkMask );

    imgCountTitleLength = ( _imgCount << imgCountShift ) 
                    + ( _titleLength & titleLengthMask ); 
    }

DocFeatures::DocFeatures( const DocFeatures& other ) 
    {
    numWords = other.numWords;
    imgCountTitleLength = other.imgCountTitleLength;
    linkCounts = other.linkCounts;
    docNorm = other.docNorm;
    }

DocFeatures& DocFeatures::operator=( const DocFeatures& other ) 
    {
    numWords = other.numWords;
    imgCountTitleLength = other.imgCountTitleLength;
    linkCounts = other.linkCounts;
    docNorm = other.docNorm;
    return *this;
    }

bool DocFeatures::operator==( const DocFeatures& other ) const 
    {
    return ( numWords == other.numWords &&
            imgCountTitleLength == other.imgCountTitleLength &&
            linkCounts == other.linkCounts &&
            docNorm == other.docNorm);
    }

uint32_t DocFeatures::getIncomingLinkCount( )
    {
    return linkCounts >> incomingLinkShift;
    }

uint32_t DocFeatures::getOutgoingLinkCount( )
    {
    return linkCounts & outgoingLinkMask;
    }

uint32_t DocFeatures::getImgCount( )
    {
    return imgCountTitleLength >> imgCountShift;
    }

uint32_t DocFeatures::getTitleLength( )
    {
    return imgCountTitleLength & titleLengthMask;
    }

// VITAL: encoding order of member variables must match decoding order!

// endcodes all member variables into buf
// returns pointer just past last written byte in buf
char * DocFeatures::encode( char* buf ) const 
    {
    // decode all unsigned integer types
    // buf = byteEncode(buf, ADD MEMBER VAR );
    // ...
    buf = byteEncode( buf, numWords );
    buf = byteEncode( buf, imgCountTitleLength );
    buf = byteEncode( buf, linkCounts );
    buf = byteEncode( buf, docNorm );
    return buf;
    }

const char * DocFeatures::decode( const char * buf ) 
    {
    // decode all unsigned integer types
    // buf = byteEncode(buf, ADD MEMBER VAR );
    // ...
    buf = byteDecode( buf, numWords );
    buf = byteDecode( buf, imgCountTitleLength );
    buf = byteDecode( buf, linkCounts );
    buf = byteDecode( buf, docNorm );
    return buf;
    }

size_t DocFeatures::byteSize( ) const 
    {
    return byteEncodingSize( numWords )
        + byteEncodingSize( imgCountTitleLength )
        + byteEncodingSize( linkCounts )
        + byteEncodingSize( docNorm );
    }

/////// WordFeatures

WordFeatures::WordFeatures( ): idf( 0.0f ) { }
WordFeatures::WordFeatures( float _idf ): idf( _idf ){ }

WordFeatures::WordFeatures( const WordFeatures& other ) 
    {
    idf = other.idf;
    }

WordFeatures& WordFeatures::operator=( const WordFeatures& other ) 
    {
    idf = other.idf;
    return *this;
    }

bool WordFeatures::operator==( const WordFeatures& other ) const 
    {
    return ( idf == other.idf ); //) &&
    }

// VITAL: encoding order of member variables must match decoding order!

// endcodes all member variables into buf
// returns pointer just past last written byte in buf
char * WordFeatures::encode( char* buf ) const 
    {
    // decode all unsigned integer types
    // buf = byteEncode(buf, ADD MEMBER VAR );
    // ...
    buf = byteEncode( buf, idf );
    return buf;
    }

const char * WordFeatures::decode( const char * buf ) 
    {
    // decode all unsigned integer types
    // buf = byteEncode(buf, ADD MEMBER VAR );
    // ...
    buf = byteDecode(buf, idf);
    return buf;
    }

size_t WordFeatures::byteSize( ) 
    {
    return byteEncodingSize( idf );
    }

/////// WordDocFeatures


WordDocFeatures::WordDocFeatures( ): termFreqAnchorCount( 0 ) { }

WordDocFeatures::WordDocFeatures( uint32_t _anchorTermCount, uint32_t _termFreq )
    {
    termFreqAnchorCount = ( _termFreq << termFreqShift ) 
                    + ( _anchorTermCount & anchorCountMask );
    }

WordDocFeatures::WordDocFeatures( const WordDocFeatures& other ) 
    {
    termFreqAnchorCount = other.termFreqAnchorCount;
    }

WordDocFeatures& WordDocFeatures::operator=( const WordDocFeatures& other ) 
    {
    termFreqAnchorCount = other.termFreqAnchorCount;
    return *this;
    }

bool WordDocFeatures::operator==( const WordDocFeatures& other ) const 
    {
    return ( termFreqAnchorCount == other.termFreqAnchorCount ); //) &&
    }

uint32_t WordDocFeatures::getTermFreq( )
    {
    return termFreqAnchorCount >> termFreqShift;
    }
        // counts number of documents linking to this doc using this term
uint32_t WordDocFeatures::getAnchorTermCount( )
    {
    return termFreqAnchorCount & anchorCountMask;
    }


// VITAL: encoding order of member variables must match decoding order!

// endcodes all member variables into buf
// returns pointer just past last written byte in buf
char * WordDocFeatures::encode( char* buf ) const 
    {
    // decode all unsigned integer types
    // buf = byteEncode(buf, ADD MEMBER VAR );
    // ...
    buf = byteEncode( buf, termFreqAnchorCount );    
    return buf;
    }

const char * WordDocFeatures::decode( const char * buf ) 
    {
    // decode all unsigned integer types
    // buf = byteEncode(buf, ADD MEMBER VAR );
    // ...
    buf = byteDecode( buf, termFreqAnchorCount );
    return buf;
    }