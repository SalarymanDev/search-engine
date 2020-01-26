#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "src/libs/serialize/Buffer.h"
#include <type_traits>
#include <iostream>

namespace BBG {
    template<typename T>
    void serialize(const T val, Buffer& bytes) {
        static_assert(std::is_fundamental<T>::value, "Serialize error: Missing overload!");

        size_t size = sizeof(T);
        char buff[size];
        *((T*)buff) = val;
        bytes.append(buff, size);
    }

    template<typename T>
    void deserialize(Buffer& bytes, T& val) {
        static_assert(std::is_fundamental<T>::value, "Deserialize error: Missing overload!");

        const char* buff = bytes.cursor(sizeof(T));
        if (!buff)
            throw "This data is shit";

        val = *((T*)buff);
        bytes.increment_cursor(sizeof(T));
    }
}

#endif