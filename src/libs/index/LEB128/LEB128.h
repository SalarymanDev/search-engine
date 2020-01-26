#ifndef LEB128_H_
#define LEB128_H_
#include <stdlib.h> // size_t

namespace BBG {

    // encoding and decoding doesn't make sense for floating values.
    // Below are same as copying to/from buffer. Only here to provide consistent interface

    // Encode floating types
    char * byteEncode(char* buf, float value);
    char * byteEncode(char* buf, double value);
    // Decode floating types
    const char * byteDecode(const char* buf, float& value);
    const char * byteDecode(const char* buf, double& value);

    // Actual encoding only works on integer types

    // encodes an UNSIGNED int of any length
    // Returns first bytes past last of value
    template <typename T>
    char * byteEncode(char* buf, T value );

    // NOTE: Assuming all bytes of result are zero!!
    //      If that cannot be garunteed, use result to extrac bytes, 
    //      then set value to result.
    template <typename T>
    const char * byteDecode(const char* buf, T& result);

    // THe following are used to calculate number of bytes required for encoding

    size_t byteEncodingSize(float value);

    size_t byteEncodingSize(double value);

    template <typename T>
    size_t byteEncodingSize(T value);
}

template <typename T>
char * BBG::byteEncode(char* buf, T value ) {
    do {
        // low order 7 bits of value;
        unsigned char byte = value & 0b01111111; 
        value >>= 7;
        // If more bytes, set high order bit of byte to 1. o.w. it stays 0
        if (value != 0) 
            byte += 0b10000000;
        *buf = byte;
        ++buf;
    } while (value != 0 );
    return buf;
}

// NOTE: Assuming all bytes of result are zero!!
//      If that cannot be garunteed, use result to extrac bytes, 
//      then set value to result.
template <typename T>
const char * BBG::byteDecode(const char* buf, T& result) {
    size_t shift = 0;
    while(true) {
        // next byte in input;
        unsigned char byte = *buf++; 
                // low order 7 bits of byte
        result |= ( (byte & 0b01111111) << shift );
            // high order bit of byte
        if ( (byte & 0b10000000) == 0)
            break;
        shift += 7;
    }
    return buf;
}

template <typename T>
size_t BBG::byteEncodingSize(T value) {
    size_t numBytes = 0;
    do {
        value >>= 7;
        ++numBytes;
    } while (value != 0 );
    return numBytes;
}

#endif
