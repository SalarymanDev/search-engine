#include "src/libs/utils/utils.h"

bool BBG::strcmp(const char* left, const char* right) {
    if (!left || !right)
        return false;
    while (*left && *right)
        if (*left++ != *right++)
            return false;

    // both are '\0'
    return *left == *right;
}

bool BBG::strncmp(const char* left, const char* right, size_t len) {
    size_t pos = 0;
    if (!left || !right)
        return false;
    
    while (*left && *right && pos++ < len)
        if (*left++ != *right++)
            return false;

    // we either checked len chars or both are '\0'
    return pos == len || *left == *right;
}

char* BBG::strcpy(char* destination, const char* source) {
    char* temp = destination;
    while (*source)
        *temp++ = *source++;

    *temp = '\0';
    return destination;
}

size_t BBG::strlen(const char* str) {
    size_t length = 0;
    while(*str++) 
        ++length;
    return length;
}

size_t BBG::FileSize( int fd ) {
    struct stat fileInfo;
    fstat( fd, &fileInfo );
    return fileInfo.st_size;
}

size_t RoundUp(size_t length, size_t boundary) {
    // Round up to the next multiple of the boundary, which
    // must be a power of 2.
    static const size_t oneless = boundary - 1,
        mask = ~( oneless );
    return ( length + oneless ) & mask;
}
