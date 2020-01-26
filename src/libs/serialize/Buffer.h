#ifndef Buffer_H
#define Buffer_H

#include <stddef.h>

namespace BBG {
    class Buffer {
    private:
        size_t _size;
        size_t _capacity;
        char* _data;
        size_t _cursor = 0;

        void grow();

    public:
        Buffer();
        Buffer(size_t size);
        Buffer(const char* data, size_t size);
        Buffer(const Buffer& other);
        Buffer(Buffer&& other);
        ~Buffer();

        size_t size() const;
        size_t capacity() const;
        const char* data() const;
        char* modifyData() const;
        void clear();
        bool empty() const;

        void reset_cursor();
        const char* cursor(size_t size = 1);
        void increment_cursor(size_t size);
        void decrement_cursor(size_t size);

        void append(const char* buff, size_t size);
        void push_back(const char val);
        void pop_back();

        Buffer& operator+= (const char* other);
        Buffer& operator+= (const char other);
        Buffer& operator+= (const Buffer& other);
        Buffer& operator=(const Buffer& other);
        Buffer operator=(Buffer&& other);
        bool operator==(const Buffer& other) const;
        bool operator==(const char* other) const;
        bool operator!=(const Buffer& other) const;
        bool operator!=(const char* other) const;
    };
}

#endif