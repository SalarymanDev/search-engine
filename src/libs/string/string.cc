#include "src/libs/string/string.h"
#include "src/libs/utils/utils.h"

using BBG::string;
using BBG::Buffer;

// Returns the smaller of the two integers
size_t min(size_t a, size_t b) {
   return a < b ? a : b;  
}


string::string() {
    _data.push_back('\0');
}


string::string(const string& str) {
    _data = str._data;
}


string::string(const char* s) {
    size_t len = strlen(s);

    _data.reserve(len + 1);  
   for (size_t i = 0; i < len; ++i)
      _data.push_back(s[i]);

    _data.push_back('\0');
}


string::string( const char* buff, size_t size ) {
    _data.reserve(size + 1);
    for (size_t i = 0; i < size; ++i)
        _data.push_back(buff[i]);
    _data.push_back('\0');
}


string& string::operator=(const string& other) {
    _data = other._data;
    return *this;
}


string& string::operator=(const char* s) {
    size_t len = strlen(s);

    _data.clear();
    _data.reserve(len + 1);

   for (size_t i = 0; i < len; ++i)
      _data.push_back(s[i]);
    
    _data.push_back('\0');

    return *this;
}


size_t string::size() const {
    return _data.size() - 1;
}


void string::resize(size_t newSize) {
    _data.resize(newSize + 1);
    _data.back() = '\0';
}


void string::resize(size_t newSize, char c) {
    _data.pop_back(); // remove '\0'
    _data.resize(newSize + 1, c);
    _data.back() = '\0';
}


size_t string::capacity() const {
    return _data.capacity() - 1;
}


void string::reserve(size_t n) {
    _data.reserve(n + 1);
}


void string::clear() {
    _data.clear();
    _data.push_back('\0');
}


bool string::empty() const {
    return _data.size() <= 1;
}


char& string::operator[](size_t pos) {
    assert(pos < size());
    return _data[pos];
}


const char& string::operator[](size_t pos) const {
    assert(pos < size());
    return _data[pos];
}


char& string::back() {
    assert(!empty());
    return _data[_data.size() - 2];
}


const char& string::back() const {
    assert(!empty());
    return _data[_data.size() - 2];
}   


char& string::front() {
    assert(!empty());
    return _data.front();
}


const char& string::front() const {
    assert(!empty());
    return _data.front();
}


const char* string::begin() const {
    return _data.begin();  
}


const char* string::end() const {
    return _data.end() - 1;  
}


string& string::operator+=(const string& other) {
    append(other);
    return *this;
}


string& string::operator+=(const char* s) {
    _data.pop_back(); // remove '\0'

    for (size_t i = 0; s[i]; ++i)
        _data.push_back(s[i]);
    _data.push_back('\0');
    return *this;
}


string& string::operator+=(char c) {
    push_back(c);
    return *this;
}


string& string::append(char* buff, int length) {
    _data.pop_back(); // remove '\0'
    for (int i = 0; i < length; ++i)
        _data.push_back(buff[i]);
    _data.push_back('\0');
    return *this;
}


string& string::append(const char* buff, int length) {
    _data.pop_back(); // remove '\0'
    for (int i = 0; i < length; ++i)
        _data.push_back(buff[i]);
    _data.push_back('\0');  
    return *this;
}


string& string::append(const string& other) {
    _data.pop_back();
    for (char c : other._data)
        _data.push_back(c);
    return *this;
}


void string::push_back(char c) {
    _data.back() = c;
    _data.push_back('\0');
}


void string::pop_back() {
    assert(size()); 
    _data.pop_back();
    _data[size()] = '\0';
}


const char* string::c_str() const {
    return _data.begin();
}


bool string::operator==(const string& other) const {
    if (this == &other)
        return true;
    
    if (size() != other.size())
        return false;
    
    for (size_t i = 0; i < size(); ++i)
        if (_data[i] != other._data[i])
            return false;  
    return true;
}


bool string::operator==(const char* s) const {
    if (size() != strlen(s))
        return false;

    for (size_t i = 0; i < size(); ++i)
        if (_data[i] != s[i])
            return false;
    
    return true;
}


bool string::operator!=(const string& other) const {
    return !(*this == other);  
}


bool string::operator!=(const char* s) const {
    return !(*this == s);
}


bool string::operator<(const string& other) const {
    if (this == &other)
        return false;
    
    size_t this_size = size(), other_size = other.size();  
    size_t min_size = min(this_size, other_size);
    for (size_t i = 0; i < min_size; ++i)
        if (_data[i] != other._data[i])
            return _data[i] < other._data[i];  
    return this_size < other_size;  
}


bool string::operator>(const string& other) const {
    if (this == &other)
        return false;
    
    size_t this_size = size(), other_size = other.size();  
    size_t min_size = min(this_size, other_size);
    for (size_t i = 0; i < min_size; ++i)
        if (_data[i] != other._data[i])
           return _data[i] > other._data[i];  
    return this_size > other_size;  
}


bool string::operator<=(const string& other) const {
    return !(*this > other); 
}


bool string::operator>=(const string& other) const {
    return !(*this < other); 
}


size_t string::find(const string& str, size_t pos) const {
    return find(str.begin(), pos, str.size());
}


size_t string::find(const char* s, size_t pos) const {
    return find(s, pos, strlen(s)); // s is null-terminated
}


size_t string::find(const char* buff, size_t pos, size_t n) const {
    size_t found = npos;

    if (n <= size() - pos) {
        size_t max = size() - n + 1; // last possible start
        const char* start = _data.begin();

        for (size_t i = pos; i < max; ++i) {
            if (strncmp(start + i, buff, n)) {
                found = i;
                break;
            }
        }
    }

    return found;
}


size_t string::find(char c, size_t pos) const {
    size_t found = npos;

    for (size_t i = pos; i < size(); ++i) 
        if (_data[i] == c) {
            found = i;
            break;
        }
    
    return found;
}


string string::substr(size_t pos, size_t len) const {
    string res;
    size_t _size = size();
    if (pos < _size) {
        size_t res_size = min(_size - pos, len);
        res.append(_data.begin() + pos, res_size);
    }
    return res;
}


size_t string::contains(char item) const {
    size_t pos = 0, count = 0;
    while ((pos = find(item, pos + 1)) != string::npos)
        ++count;
    return count;
}

void string::lower() {
    size_t _size = size();
    for (size_t i = 0; i < _size; ++i)
        if (64 < _data[i] && _data[i] < 91)
            _data[i] += 32;
}

bool string::isAlphaNum() {
    size_t _size = size();
    for (size_t i = 0; i < _size; ++i) {
        // Chars '\0' to '/' in the ascii table
        if (_data[i] < 48)
            return false;
        // Chars ':' to '@'
        if (_data[i] > 57 && _data[i] < 65)
            return false;
        // Chars '[' to '`'
        if (_data[i] > 90 && _data[i] < 97)
            return false;
        // Chars '{' to 'DEL'
        if (_data[i] > 122)
            return false;
    }
    return true;
}

size_t string::findFirstOf(vector<char>& vec, size_t pos) {
    size_t res = npos, tmp;
    for (size_t i = 0; i < vec.size(); ++i) {
        tmp = find(vec[i], pos);
        res = tmp < res ? tmp : res;
    }
    return res;
}

std::ostream& BBG::operator<<(std::ostream& os, const string& str) {
    for (size_t i = 0; i < str.size(); ++i)
        os << str[i];
    return os;
}


const char* BBG::getLine(const char* buff, string& out) {
    // Strip and line-ending characters from the beginning of the string
    while (*buff && (*buff == '\r' || *buff == '\n')) ++buff;
    
    const char* cur = buff;

    // Advance cur to the first \0, \r, or \n character
    while (*cur && *cur != '\r' && *cur != '\n') ++cur;

    out.clear();

    // Append all characters in the range [buff, cur),
    // the append method takes care of \0 in the string
    out.append(buff, cur - buff);

    // Advance cur to the first non \n or \r character
    while (*cur && (*cur == '\r' || *cur == '\n')) ++cur;

    return cur;
}


void BBG::swap( string& a, string &b ) {
    BBG::swap(a._data, b._data);
}

void BBG::serialize(const string& str, Buffer& bytes) {
    bytes.append(str.c_str(), str.size() + 1);
}

void BBG::deserialize(Buffer& bytes, string& str) {
    // Validate data is not corrupt
    const char* cursor = bytes.cursor(1);
    if (!cursor) throw "This data sucks!";
    size_t stringLength = 0;
    while (cursor && *cursor) {
        ++stringLength;
        bytes.increment_cursor(1);
        cursor = bytes.cursor(1);
        if (!cursor) throw "This data sucks!";
    }
    bytes.decrement_cursor(stringLength);

    // Copy data
    str = string(bytes.cursor());
    bytes.increment_cursor(str.size() + 1);
}

string BBG::operator+(const char* cstr, const string& str) {
    string res = cstr;
    res += str;
    return res;
}

string BBG::operator+(const string& str, const char* cstr) {
    string res = str;
    res += cstr;
    return res;
}

string BBG::operator+(const string& strA, const string& strB) {
    return strA.c_str() + strB;
}


string BBG::to_string(uint32_t num) {
    char buff[32];
    sprintf(buff, "%u", num);
    return string(buff);
}

string BBG::to_string(int num) {
    char buff[32];
    sprintf(buff, "%d", num);
    return string(buff);
}

string BBG::to_string(double num) {
    char buff[64];
    sprintf(buff, "%f", num);
    return string(buff);
}

string BBG::to_string(float num) {
    char buff[32];
    sprintf(buff, "%f", num);
    return string(buff);
}

string BBG::to_string(long num) {
    char buff[64];
    sprintf(buff, "%ld", num);
    return string(buff);
}

string BBG::to_string(unsigned long num) {
    char buff[64];
    sprintf(buff, "%lu", num);
    return string(buff);
}

string BBG::to_string(vector<string>& vec) {
    string out;
    if (vec.empty())
        return out;
    for (size_t i = 0; i < vec.size() - 1; ++i)
        out += vec[i] + " ";
    out += vec.back();
    return out;
}