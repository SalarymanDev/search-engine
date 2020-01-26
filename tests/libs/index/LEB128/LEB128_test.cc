#define CATCH_CONFIG_MAIN
#include "tests/catch.hpp"
#include "src/libs/index/LEB128/LEB128.h"
#include "src/libs/vector/vector.h"

using namespace BBG;

TEST_CASE("simple char going over byte boundry", "[LEB128]") {

    vector<unsigned char> vec;
    char * lebBuf = new char[1000];
    char * encodePosition = lebBuf;
    size_t start = 125;
    for( size_t i = 0; i < 10; ++i) {
        unsigned char value = start + i;
        vec.push_back (value);
        // NOTE: encode encodePosition + number of bytes used
        encodePosition = byteEncode(encodePosition, value );
    }
    // mild sanity check make sure LEB is actually doing something. 
    // Non-decoded access should give garbage 
    REQUIRE( vec.size() * sizeof(char) < ( encodePosition - lebBuf ) );

    const char * decodePosition = lebBuf;
    for( size_t i = 0; i < 10; ++i) {
        unsigned char value = 0;
        // NOTE: decode returns decodePosition + number of bytes used
        decodePosition = byteDecode(decodePosition, value);
        REQUIRE(vec[i] == value );
    }
    delete[] lebBuf;
}


TEST_CASE("large int values", "[LEB128]") {

    vector<unsigned int> vec;
    char * lebBuf = new char[1000];
    char * encodePosition = lebBuf;
    int start = 0xFFFF;
    for( size_t i = 0; i < 10; ++i) {
        unsigned int value = start + i;
        vec.push_back (value);
        // NOTE: encode encodePosition + number of bytes used
        encodePosition = byteEncode(encodePosition, value);
    }

    // mild sanity check make sure LEB is actually doing something. 
    // Non-decoded access should give garbage 
    REQUIRE(lebBuf[5] != vec[5]);

    const char * decodePosition = lebBuf;
    for( size_t i = 0; i < 10; ++i) {
        unsigned int value = 0;
        // NOTE: decode returns decodePosition + number of bytes used
        decodePosition = byteDecode( decodePosition, value);
        REQUIRE(vec[i] == value );
    }
    delete[] lebBuf;
}


TEST_CASE("doubles", "[LEB128]") {

    vector<double> vec;
    char * lebBuf = new char[1000];
    char * encodePosition = lebBuf;
    double start = 0xFFFFFF + 0.5;
    for( size_t i = 0; i < 10; ++i) {
        double value = start / (i+1);
        vec.push_back(value );
        // NOTE: encode encodePosition + number of bytes used
        encodePosition = byteEncode(encodePosition, value);
    }

    // mild sanity check make sure LEB is actually doing something. 
    // Non-decoded access should give garbage 
    REQUIRE(lebBuf[5] != vec[5]);

    const char * decodePosition = lebBuf;
    for( size_t i = 0; i < 10; ++i) {
        double value = 0;
        // NOTE: decode returns currentByte + number of bytes used
        decodePosition = byteDecode( decodePosition, value);
        REQUIRE(vec[i] == value );
    }
    delete[] lebBuf;
}


TEST_CASE("crossing byte boundries", "[LEB128]") {

    vector<unsigned int> vec;
    char * lebBuf = new char[10000 * 4];
    char * encodePosition = lebBuf;
    
    int start = (1 << 21 ) - 5000;
    // goes from needing 3 byte to 4 bytes after i = 5000
    size_t numBytesNeeded = 5000 * 3 + 5000 * 4;
    
    for( size_t i = 0; i < 10000; ++i) {
        unsigned int value = start + i;
        vec.push_back (value);
        // NOTE: encode encodePosition + number of bytes used
        encodePosition = byteEncode(encodePosition, value);
    }
    REQUIRE( encodePosition - lebBuf == numBytesNeeded);

    // mild sanity check make sure LEB is actually doing something. 
    // Non-decoded access should give garbage 
    REQUIRE(lebBuf[5] != vec[5]);

    const char * decodePosition = lebBuf;
    for( size_t i = 0; i < 10000; ++i) {
        unsigned int value = 0;
        // NOTE: decode returns decodePosition + number of bytes used
        decodePosition = byteDecode( decodePosition, value);
        REQUIRE(vec[i] == value );
    }
    REQUIRE( decodePosition - lebBuf == numBytesNeeded);
    
    delete[] lebBuf;
}