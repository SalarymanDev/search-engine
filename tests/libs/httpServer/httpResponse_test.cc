#include "src/libs/httpServer/HttpResponse.h"
#include "tests/catch.hpp"
#include <iostream>

using BBG::HttpResponse;
using BBG::string;
using std::cout;
using std::endl;

TEST_CASE("Http Response to string with body", "[HttpResponse]") {
    HttpResponse response;
    response.status = HttpResponse::StatusCode::OK;
    response.headers["Content-Type"] = "text/html";
    response.body.append("<html><head></head><body></body></html>", 39);
    string actual = response.toString();
    string expected = "HTTP/1.0 200 OK\r\nContent-Length: 39\r\nContent-Type: text/html\r\n\r\n<html><head></head><body></body></html>";
    REQUIRE(actual == expected);
}

TEST_CASE("Http Response to string no body", "[HttpResponse]") {
    HttpResponse response;
    response.status = HttpResponse::StatusCode::OK;
    response.headers["Content-Type"] = "text/html";
    string actual = response.toString();
    string expected = "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n";
    REQUIRE(actual == expected);
}

TEST_CASE("Http Response from string with body", "[HttpResponse]") {
    HttpResponse expected;
    expected.status = HttpResponse::StatusCode::OK;
    expected.isValid = true;
    expected.body.append("<html><head></head><body></body></html>", 39);
    expected.headers["Content-Type"] = "text/html";
    expected.headers["Content-Length"] = "39";

    string response("HTTP/1.0 200 OK\r\nContent-Length: 39\r\nContent-Type: text/html\r\n\r\n<html><head></head><body></body></html>");
    HttpResponse actual(response);

    REQUIRE(actual.status == expected.status);
    REQUIRE(actual.body == expected.body);
    REQUIRE(actual.headers == expected.headers);
    REQUIRE(actual.isValid == expected.isValid);
}

TEST_CASE("Http Response from string no body", "[HttpResponse]") {
    HttpResponse expected;
    expected.status = HttpResponse::StatusCode::OK;
    expected.isValid = true;
    expected.headers["Content-Type"] = "text/html";

    string response("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n");
    HttpResponse actual(response);

    REQUIRE(actual.status == expected.status);
    REQUIRE(actual.body == expected.body);
    REQUIRE(actual.headers == expected.headers);
    REQUIRE(actual.isValid == expected.isValid);
}