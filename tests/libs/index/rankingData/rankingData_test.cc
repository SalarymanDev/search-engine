#define CATCH_CONFIG_MAIN
#include "src/libs/index/rankingData/rankingData.h"
#include "tests/catch.hpp"

using namespace BBG;

TEST_CASE("Simple DocFeatures test", "[rankingData]") {

    uint32_t _numWords = 10;
    uint32_t _titleLength = 20;
    uint32_t _imgCount = 30;
    size_t _incomingLinkCount = 40; 
    size_t _outgoingLinkCount = 50;
    float _docNorm = 60.0f;

    DocFeatures docFeats(_numWords, _titleLength, _imgCount, 
                        _incomingLinkCount, _outgoingLinkCount, _docNorm );
    
    REQUIRE( docFeats.numWords == _numWords );
    REQUIRE( docFeats.getTitleLength( ) == _titleLength );
    REQUIRE( docFeats.getImgCount( ) == _imgCount );
    REQUIRE( docFeats.getIncomingLinkCount( ) == _incomingLinkCount );
    REQUIRE( docFeats.getOutgoingLinkCount( ) == _outgoingLinkCount );
    REQUIRE( docFeats.docNorm == _docNorm );
}

TEST_CASE("Simple WordFeatures test", "[rankingData]") {

    float _idf = 60.0f;

    WordFeatures wordFeats( _idf );
    
    REQUIRE( wordFeats.idf == _idf );
}

TEST_CASE("Simple WordDocFeatures test", "[rankingData]") {

    uint32_t _anchorCount = 10;
    uint32_t _termFreq = 20;
    
    WordDocFeatures wordDocFeats ( _anchorCount, _termFreq );
    
    REQUIRE( wordDocFeats.getTermFreq( ) == _termFreq );
    REQUIRE( wordDocFeats.getAnchorTermCount( ) == _anchorCount );
}