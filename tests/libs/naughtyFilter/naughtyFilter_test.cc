#include "src/libs/naughtyFilter/NaughtyFilter.h"
#include "tests/catch.hpp"

using BBG::NaughtyFilter;

void setupEnvironment() {
    putenv("BBG_STOPWORDS=src/libs/naughtyFilter/stopwords.txt");
    putenv("BBG_BLACKLIST=src/libs/naughtyFilter/blacklist.txt");
}

TEST_CASE("Should read from blacklist.", "[naughtyFilter]") {
    setupEnvironment();
    NaughtyFilter nsfw(false);
    REQUIRE(nsfw.isNaughty("fuck"));
    REQUIRE(!nsfw.isNaughty("test"));
}

TEST_CASE("Should read from stopwords list.", "[naughtyFilter]") {
    setupEnvironment();
    NaughtyFilter stopwords(true);
    REQUIRE(stopwords.isNaughty("your"));
    REQUIRE(!stopwords.isNaughty("garage"));
}