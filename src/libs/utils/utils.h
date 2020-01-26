#ifndef UTILS_H
#define UTILS_H

#include <cstddef> // size_t
#include "src/libs/serialize/Buffer.h"
#include "src/libs/serialize/serialize.h"
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

namespace BBG {
    // Returns true if left and right match exactly
    bool strcmp(const char* left, const char* right);

    // Returns true if left and right match up to len characters (or exactly if they are shorter)
    bool strncmp(const char* left, const char* right, size_t len);

    // Copies source into destination
    char* strcpy(char* destination, const char* source);

    // Returns the length of str, without counting \0
    size_t strlen(const char* str);

    template <typename T>
    void swap(T & a, T & b);

    template <typename T>
    void write_data(const T & data, const char* fname);

    size_t FileSize( int fd );

    size_t RoundUp( size_t length, size_t boundary );
}

template <typename T>
void BBG::swap(T & a, T & b) {
    T tmp = a;
    a = b; b = tmp;
}

template <typename T>
void BBG::write_data(const T & data, const char* fname) {
    FILE *file = fopen(fname, "w");
    if (!file) return;

    BBG::Buffer bytes;
    serialize(data, bytes);
    fwrite(bytes.data(), 1, bytes.size(), file);
    fclose(file);
}

#endif
