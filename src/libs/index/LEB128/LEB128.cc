#include "src/libs/index/LEB128/LEB128.h"

// Encode floating types
char * BBG::byteEncode(char* buf, float value) {
    *(float *) (buf) = value;
    return buf += sizeof(float);
}

char * BBG::byteEncode(char* buf, double value) {
    *(double *) (buf) = value;
    return buf += sizeof(double);
}

// Decode floating types
const char * BBG::byteDecode(const char* buf, float& value) {
    value = *(float *) (buf);
    return buf += sizeof(float);
}

const char * BBG::byteDecode(const char* buf, double& value) {
    value = *(double *) (buf);
    return buf += sizeof(double);
}

size_t BBG::byteEncodingSize(float value) {
    return sizeof(float);
}

size_t BBG::byteEncodingSize(double value) {
    return sizeof(double);
}
