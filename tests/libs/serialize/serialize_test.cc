#include "src/libs/serialize/serialize.h"
#include "tests/catch.hpp"

using BBG::Buffer;
using BBG::serialize;
using BBG::deserialize;

TEST_CASE("Integer Test", "[serialize]") {
    int expected = 5, actual;
    Buffer bytes;
    serialize(expected, bytes);
    deserialize(bytes, actual);
    REQUIRE(actual == expected);
}

TEST_CASE("Size_t Test", "[serialize]") {
    size_t expected = 500, actual;
    Buffer bytes;
    serialize(expected, bytes);
    deserialize(bytes, actual);
    REQUIRE(actual == expected);
}

TEST_CASE("Double Test", "[serialize]") {
    double expected = 500.5987, actual;
    Buffer bytes;
    serialize(expected, bytes);
    deserialize(bytes, actual);
    REQUIRE(actual == expected);
}
