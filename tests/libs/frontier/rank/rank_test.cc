#include "src/libs/frontier/rank/rank.h"
#include "src/libs/string/string.h"
#include "tests/catch.hpp"

using BBG::string;
using BBG::protocolIsHttps;


TEST_CASE("getProtocol", "[frontier]") {
    string good_url("https://test.org");
    string bad_url("http://test.org");
    string empty_url("");
    string malformed_url("123");

    REQUIRE(protocolIsHttps(good_url));
    REQUIRE(!protocolIsHttps(bad_url));
    REQUIRE(!protocolIsHttps(empty_url));
    REQUIRE(!protocolIsHttps(empty_url));
}
