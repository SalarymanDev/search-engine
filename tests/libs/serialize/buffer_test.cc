#include "src/libs/serialize/Buffer.h"
#include "src/libs/utils/utils.h"
#include "tests/catch.hpp"

using BBG::Buffer;
using BBG::strcmp;

TEST_CASE("Default Buffer Constructor", "[Buffer]") {
    Buffer buff;
    REQUIRE(buff.size() == 0);
}

TEST_CASE("Size constructor test", "[Buffer]") {
    size_t buffSize = 999;
    Buffer buff(buffSize);
    const char* data = buff.data();

    REQUIRE(buff.size() == buffSize);
    REQUIRE(buff.capacity() == buffSize);
    for (size_t i = 0; i < buffSize; ++i)
        REQUIRE(data[i] == '\0');
}

TEST_CASE("Raw Buffer and equals constructor", "[Buffer]") {
    const char* buff = "abcde";
    const char* buff2 = "aacde";
    Buffer actual(buff, 6);
    REQUIRE(strcmp(actual.data(), buff));

    char* data = actual.modifyData();
    data[1] = 'a';
    REQUIRE(strcmp(actual.data(), buff2));
}

TEST_CASE("Move constructor test", "[Buffer]") {
    Buffer actual(Buffer("abc", 3));
    Buffer expected("abc", 3);
    REQUIRE(actual == expected);
}

TEST_CASE("Copy constructor test", "[Buffer]") {
    Buffer expected("abc", 3);
    Buffer actual(expected);
    REQUIRE(actual == expected);
}

TEST_CASE("Clear test", "[Buffer]") {
    Buffer actual("abc", 3);
    Buffer expected;
    actual.clear();
    REQUIRE(actual == expected);
}

TEST_CASE("Push/Pop/empty Test", "[Buffer]") {
    Buffer buff;
    buff.push_back('a');
    REQUIRE(buff.size() == 1);
    REQUIRE(buff.data()[0] == 'a');
    
    buff.pop_back();
    REQUIRE(buff.size() == 0);
    REQUIRE(buff.empty());
}

TEST_CASE("Plus equals operator test", "[Buffer]") {
    Buffer actual("abc", 3);
    Buffer toAdd("def", 3);
    Buffer expected("abcdef", 6);
    actual += toAdd;
    REQUIRE(actual == expected);
}

TEST_CASE("Assignment operator test", "[Buffer]") {
    Buffer actual;
    Buffer expected("abc", 3);
    actual = expected;
    REQUIRE(actual == expected);
}
