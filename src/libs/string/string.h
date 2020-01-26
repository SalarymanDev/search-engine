#ifndef STRING_H
#define STRING_H

#include "src/libs/vector/vector.h"
#include "src/libs/serialize/Buffer.h"
#include <ostream>
#include <cstddef> // size_t
#include <cassert>

namespace BBG {

    class string {
    private:
        vector<char> _data;

    public:
        // Constructors

        // Constructs an empty string, with a length of zero characters.
        string( );

        // Constructs a copy of str.
        string( const string& str );

        // Copies the null-terminated character sequence (C-string) pointed by s.
        string( const char* s );

        // Copies the Buffer up to size
        string( const char* Buffer, size_t size );

        // Assignment operators
        string& operator=(const string& other);

        string& operator=(const char* s);

        // Size and capacity methods
        
        static const size_t npos = -1; // overflows to max size_t

        // Returns the length of the string, in terms of bytes.
        size_t size( ) const;

        // Resizes the string to a length of n characters.
        void resize ( size_t newSize );
        void resize ( size_t newSize, char c );

        // Returns the size of the storage space currently allocated for the 
        // string, expressed in terms of bytes.
        size_t capacity( ) const;

        // Requests that the string capacity be adapted to a planned change in 
        // size to a length of up to n characters.
        void reserve( size_t n = 0 );

        // Erases the contents of the string, which becomes an empty string 
        // (with a length of 0 characters).
        void clear( );

        // Returns whether the string is empty (i.e. whether its length is 0).
        bool empty( ) const;

        // Element Access

        // Returns a reference to the character at position pos in the string.
        char& operator[]( size_t pos );
        const char& operator[]( size_t pos ) const;

        // Returns a reference to the last character of the string.
        char& back( );
        const char& back( ) const;  

        // Returns a reference to the first character of the string.
        char& front( );
        const char& front( ) const;

        // Returns a random access iterator to the start of the string
        const char* begin( ) const;

        // Returns a random access iterator to the end of the string
        const char* end( ) const;

        // Modifiers

        // Extends the string by appending additional characters at the end of its current value:
        string& operator+=( const string& other );
        string& operator+=( const char* s );
        string& operator+=( char c );

        // Append raw bytes from a Buffer
        string& append(char* buff, int length);
        string& append(const char* buff, int length);

        // Append other string
        string& append(const string& other);

        // Appends character c to the end of the string, increasing its length by one.
        void push_back( char c );

        // Erases the last character of the string, effectively reducing its length by one.
        void pop_back( );

        // Returns a pointer to an array that contains a null-terminated sequence of characters 
        // (i.e., a C-string) representing the current value of the string object.
        const char* c_str( ) const;

        // Returns whether all the contents of *this and other are equal
        bool operator==( const string& other ) const;
        bool operator==( const char* s ) const;

        // Returns whether at least one character differs between *this and other
        bool operator!=( const string& other ) const;
        bool operator!=( const char* s ) const;

        // Returns whether *this is lexigraphically less than other
        bool operator<( const string& other ) const;

        // Returns whether *this is lexigraphically greater than other
        bool operator>( const string& other ) const;

        // Returns whether *this is lexigraphically less or equal to other
        bool operator<=( const string& other ) const;

        // Returns whether *this is lexigraphically greater or equal to other
        bool operator>=( const string& other ) const;

        // Returns the starting index of the first occurence of str
        // after location pos, or npos if it is not found
        size_t find(const string& str, size_t pos = 0) const;

        // Returns the starting index of the first occurence of null-terminated str
        // after location pos, or npos if it is not found
        size_t find(const char* s, size_t pos = 0) const;

        // Returns the starting index of the first occurence of buff (of length n)
        // after location pos, or npos if it is not found
        size_t find(const char* buff, size_t pos, size_t n) const;

        // Returns the starting index of the first occurence of c
        // after location pos, or npos if it is not found
        size_t find(char c, size_t pos = 0) const;

        // Returns first occurrence of a character in vec
        size_t findFirstOf(vector<char>& vec, size_t pos);

        // Returns a string starting at pos of length len
        string substr(size_t pos = 0, size_t len = npos) const;

        // Returns the amount of times item appears in the string
        size_t contains(char item) const;

        // Makes string lowercase
        void lower();

        // Returns true if string contains only a-z
        // Requires string is all lowercase
        bool isAlphaNum();

        friend void swap( string& a, string &b );
        friend void serialize(const string& str, Buffer& bytes);
        friend void deserialize(Buffer& bytes, string& str);
    };

    // Tells std::ostream how to print a string
    std::ostream& operator<<( std::ostream& os, const string& str );

    // Swap the contents of a and b in constant time
    void swap( string& a, string &b );

    // Stores the next line contained in buff which ends in \n or \r
    // Returns the start of the following line, or \0 if the end of buff is reached
    const char* getLine(const char* buff, string& out);
    // This method is not in utils due to circular dependecies

    void serialize(const string& str, Buffer& bytes);
    void deserialize(Buffer& bytes, string& str);

    // Returns the concatenation of cstr and str
    string operator+(const char* cstr, const string& str);
    string operator+(const string& str, const char* cstr);
    string operator+(const string& strA, const string& strB);

    // Converts the input to BBG::string
    string to_string(uint32_t u);
    
    // Converts the input to BBG::string
    string to_string(int u);
    
    // Converts the input to BBG::string
    string to_string(double u);
    
    // Converts the input to BBG::string
    string to_string(float u);
    
    // Converts the input to BBG::string   
    string to_string(long u);

    // Converts the input to BBG::string   
    string to_string(unsigned long u);

    // Converts the input to BBG::string   
    string to_string(vector<string>& vec);


} // BBG

#endif