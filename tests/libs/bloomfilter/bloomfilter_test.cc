#include "src/libs/bloomfilter/bloomfilter.h"
#include "tests/catch.hpp"

#include <string>

using BBG::Bloomfilter;

TEST_CASE("Gerneal Test", "[bloomfilter]") {
  Bloomfilter bf(0.2, 10);

  std::string data("testdata");
  bf.insert(data.c_str(), data.length());
  REQUIRE(bf.contains(data.c_str(), data.length()));

  data = "datanotinfilter";
  REQUIRE(!bf.contains(data.c_str(), data.length()));
  REQUIRE(bf.count() == 1);
}

TEST_CASE("Frontier Test", "[bloomfilter]") {
  Bloomfilter bf(0.001, 100000);

  std::string data("testdata");
  bf.insert(data.c_str(), data.length());
  REQUIRE(bf.contains(data.c_str(), data.length()));

  data = "datanotinfilter";
  REQUIRE(!bf.contains(data.c_str(), data.length()));
}
