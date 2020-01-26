#include "src/libs/serialize/Buffer.h"
#include <cstdlib>

using BBG::Buffer;

Buffer::Buffer() : _size(0), _capacity(10240) {
    _data = static_cast<char*>(malloc(_capacity));
}

Buffer::Buffer(size_t size)
    : _size(size), _capacity(size) {
    _data = static_cast<char*>(malloc(_capacity));
    for (size_t i = 0; i < _size; ++i)
        _data[i] = '\0';
}

Buffer::Buffer(const char* data, size_t size)
    : _size(size), _capacity(size) {
    _data = static_cast<char*>(malloc(_capacity));
    for (size_t i = 0; i < _size; ++i)
        _data[i] = data[i];
}

Buffer::Buffer(const Buffer& other)
    : _size(other._size), _capacity(other._capacity) {
    _data = static_cast<char*>(malloc(_capacity));
    for (size_t i = 0; i < _size; ++i)
        _data[i] = other._data[i];
}

Buffer::Buffer(Buffer&& other)
    : _size(other._size), _capacity(other._capacity), _data(other._data) {
    other._data = nullptr;
    other._size = other._capacity = 0;
}

void Buffer::grow() {
    _capacity *= 2;
    _data = static_cast<char*>(realloc(_data, _capacity));
}

Buffer::~Buffer() {
    free(_data);
}

size_t Buffer::size() const {
    return _size;
}


size_t Buffer::capacity() const {
    return _capacity;
}

void Buffer::reset_cursor() {
    _cursor = 0;
}

const char* Buffer::cursor(size_t size) {
    if (_cursor + size > _size)
        return nullptr;
    
    return _data + _cursor;
}

void Buffer::increment_cursor(size_t size) {
    _cursor += size;
}

void Buffer::decrement_cursor(size_t size) {
    if (size >= _cursor)
        _cursor = 0;
    else
        _cursor -= size;
}

char* Buffer::modifyData() const {
    return _data;
}

const char* Buffer::data() const {
    return _data;
}

void Buffer::clear() {
    _cursor = 0;
    _size = 0;
}

bool Buffer::empty() const {
    return _size == 0;
}

void Buffer::append(const char* buff, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (_size == _capacity)
            grow();
        _data[_size++] = buff[i];
    }
}

void Buffer::push_back(const char val) {
    if (_size == _capacity)
        grow();

    _data[_size++] = val;
}

void Buffer::pop_back() {
    if (_size == 0)
        return;
    --_size;
}

Buffer& Buffer::operator+= (const Buffer& other) {
    for (size_t i = 0; i < other._size; ++i)
        push_back(other._data[i]);
    return *this;
}

Buffer& Buffer::operator+= (const char* other) {
    for (; *other; ++other)
        push_back(*other);
    return *this;
}

Buffer& Buffer::operator+= (const char other) {
    push_back(other);
    return *this;
}

Buffer& Buffer::operator=(const Buffer& other) {
    _size = other._size;
    _capacity = other._capacity;
    _data = static_cast<char*>(malloc(_capacity));
    for (size_t i = 0; i < _size; ++i)
        _data[i] = other._data[i];
    return *this;
}

Buffer Buffer::operator=(Buffer&& other) {
    _data = other._data;
    _size = other._size;
    _capacity = other._capacity;
    other._data = nullptr;
    other._size = other._capacity = 0;
    return *this;
}

bool Buffer::operator==(const Buffer& other) const {
    if (_size != other._size)
        return false;

    for (size_t i = 0; i < _size; ++i)
        if (_data[i] != other._data[i])
            return false;

    return true;
}

bool Buffer::operator==(const char* other) const {
    size_t i = 0;
    while (*other && i < _size)
        if (*other++ != _data[i])
            return false;

    if (i != _size) return false;

    return true;
}

bool Buffer::operator!=(const Buffer& other) const {
    return !(*this == other);
}

bool Buffer::operator!=(const char* other) const {
    return !(*this == other);
}
