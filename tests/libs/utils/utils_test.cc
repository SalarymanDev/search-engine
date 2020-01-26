#include "src/libs/utils/utils.h"
#include "tests/catch.hpp"

using BBG::strcmp;
using BBG::strncmp;
using BBG::strcpy;
using BBG::strlen;
using BBG::swap;

TEST_CASE("String compare is equal.", "[utils]") {
    const char* first = "This is a test.";
    const char* second = first;

    REQUIRE(strcmp(first, second));
}

TEST_CASE("String compare is not equal for strings of different length.", "[utils]") {
    const char* first = "This is a test.";
    const char* second = "This is a test that is a different length.";

    REQUIRE(!strcmp(first, second));
}

TEST_CASE("String compare is not equal for strings of same length.", "[utils]") {
    const char* first = "Test A";
    const char* second = "Test B";

    REQUIRE(!strcmp(first, second));
}

TEST_CASE("String compare is equal up to certain length.", "[Utils]") {
    const char* first = "Test A";
    const char* second = first;
    REQUIRE(strncmp(first, second, strlen(first)));

    second = "Test ABC";
    REQUIRE(strncmp(first, second, strlen(first)));
    REQUIRE(!strncmp(first, second, strlen(second)));

    first = "ABCDEFAB";
    second = "DEFABCDE";

    REQUIRE(strncmp(first, second + 3, 3));
    REQUIRE(!strncmp(first, second + 3, 6));

    REQUIRE(strncmp(first + 3, second, 3));
    REQUIRE(!strncmp(first + 3, second, 6));
}

TEST_CASE("String compare is equal up to certain length, which is longer than both strings", "[Utils]") {
    const char* first = "This is test A";
    const char* second = first;
    REQUIRE(strncmp(first, second, 2 * strlen(first)));
}

TEST_CASE("String copy", "[Utils]") {
    char buffer[15];
    const char* expected = "This is a test";
    strcpy(buffer, expected);
    REQUIRE(strcmp(buffer, expected));
}

TEST_CASE("String length", "[utils]") {
    const char* str = "Should be length of 22";
    REQUIRE(strlen(str) == 22);
}

TEST_CASE("swap size_t", "[utils]") {
    size_t a = 5;
    size_t b = 9;
    swap(a, b);

    REQUIRE(a == 9);
    REQUIRE(b == 5);
}

TEST_CASE("swap ptr template", "[utils]") {
    size_t a = 5;
    size_t b = 9;

    size_t * a_ptr = &a;
    size_t * b_ptr = &b;

    REQUIRE(*a_ptr == 5);
    REQUIRE(*b_ptr == 9);

    swap(a_ptr, b_ptr);

    REQUIRE(*a_ptr == 9);
    REQUIRE(*b_ptr == 5);
}
