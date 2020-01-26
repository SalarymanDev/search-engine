#include "src/libs/httpServer/HttpRequest.h"
#include "tests/catch.hpp"

using BBG::HttpRequest;
using BBG::string;

TEST_CASE("Parse constructor with no body", "[HttpRequest]") {
    HttpRequest request("GET / HTTP/1.0\r\nUser-Agent: test\r\nHost: localhost:8080\r\nAccept: */*\r\n\r\n");
    REQUIRE(request.path == "/");
    REQUIRE(request.httpVersion == "HTTP/1.0");
    REQUIRE(request.method == "GET");
    REQUIRE(request.headers.contains("Host"));
    REQUIRE(request.headers["Host"] == "localhost:8080");
    REQUIRE(request.headers.contains("User-Agent"));
    REQUIRE(request.headers["User-Agent"] == "test");
    REQUIRE(request.headers.contains("Accept"));
    REQUIRE(request.headers["Accept"] == "*/*");
}

TEST_CASE("Parse constructor with body", "[HttpRequest]") {
    HttpRequest request("GET / HTTP/1.0\r\nUser-Agent: test\r\nHost: localhost:8080\r\nAccept: */*\r\nContent-Length: 13\r\n\r\n{\"test\":true}");
    REQUIRE(request.path == "/");
    REQUIRE(request.httpVersion == "HTTP/1.0");
    REQUIRE(request.method == "GET");
    REQUIRE(request.headers.contains("Host"));
    REQUIRE(request.headers["Host"] == "localhost:8080");
    REQUIRE(request.headers.contains("User-Agent"));
    REQUIRE(request.headers["User-Agent"] == "test");
    REQUIRE(request.headers.contains("Accept"));
    REQUIRE(request.headers["Accept"] == "*/*");
    REQUIRE(request.headers.contains("Content-Length"));
    REQUIRE(request.headers["Content-Length"] == "13");
}
