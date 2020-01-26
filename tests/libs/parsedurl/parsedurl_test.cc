#include "src/libs/parsedurl/ParsedUrl.h"
#include "src/libs/utils/utils.h"
#include "tests/catch.hpp"

using BBG::ParsedUrl;
using BBG::strcmp;
using BBG::string;

TEST_CASE("String Constructor", "[ParsedUrl]") {
    string url("http://localhost:8080/api/");
    ParsedUrl parsedUrl(url);

    bool urlIsGood = parsedUrl.CompleteUrl == url;
    REQUIRE(urlIsGood);
    REQUIRE(strcmp(parsedUrl.Service, "http"));
    REQUIRE(strcmp(parsedUrl.Host, "localhost"));
    REQUIRE(strcmp(parsedUrl.Port, "8080"));
    REQUIRE(strcmp(parsedUrl.Path, "api/"));
}

TEST_CASE("C-String Constructor", "[ParsedUrl]") {
    const char* url = "http://localhost:8080/api/";
    ParsedUrl parsedUrl(url);

    bool urlIsGood = parsedUrl.CompleteUrl == url;
    REQUIRE(urlIsGood);
    REQUIRE(strcmp(parsedUrl.Service, "http"));
    REQUIRE(strcmp(parsedUrl.Host, "localhost"));
    REQUIRE(strcmp(parsedUrl.Port, "8080"));
    REQUIRE(strcmp(parsedUrl.Path, "api/"));
}

TEST_CASE("Wikipedia port should be null", "[parsedurl]") {
    string url("https://en.wikipedia.org/wiki/Main_Page");
    ParsedUrl parsed(url);

    REQUIRE(parsed.CompleteUrl == url);

    REQUIRE(strcmp(parsed.Service, "https"));
    REQUIRE(strcmp(parsed.Host, "en.wikipedia.org"));
    REQUIRE(strcmp(parsed.Port, ""));
    REQUIRE(strcmp(parsed.Path, "wiki/Main_Page"));


}