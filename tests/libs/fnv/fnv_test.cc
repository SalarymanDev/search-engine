#include "src/libs/string/string.h"
#include "src/libs/fnv/fnv.h"
#include "tests/catch.hpp"

using namespace BBG;

TEST_CASE() {
  string data("testdata");
  REQUIRE(fnv(data.c_str(), data.size()) == 0xcda675678e81d205);
  REQUIRE(fnv(data.c_str(), data.size()) == fnv(&data, 0));
}
