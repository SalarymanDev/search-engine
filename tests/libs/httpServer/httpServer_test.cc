#include "src/libs/httpServer/HttpServer.h"
#include "src/libs/connection/connection.h"
#include "tests/catch.hpp"
#include <unistd.h>
#include <iostream>

using BBG::HttpServer;
using BBG::HttpRequest;
using BBG::HttpResponse;
using BBG::Connection;
using BBG::vector;
using BBG::string;
using std::cout;
using std::endl;

HttpResponse *endpoint(HttpRequest& request) {
    HttpResponse *response = new HttpResponse();
    response->status = HttpResponse::StatusCode::OK;
    response->headers["Content-Length"] = "13";
    response->headers["Content-Type"] = "text/html";
    response->body.append("<html></html>", 13);
    return response;
}

TEST_CASE("Server Constructor with port binding", "[HttpServer]") {
    HttpServer server("tests/libs/httpServer/wwwroot", 8090, 25, 5);
    server.run();
    usleep(2500);
    server.stop();
    usleep(2500);
}

TEST_CASE("HTTP Server-Client Client Disconnected", "[HttpServer]") {
    HttpServer server("tests/libs/httpServer/wwwroot", 8091, 25, 5);
    server.run();
    usleep(2500);

    Connection conn;
    int result = conn.connect("localhost", 8091);
    REQUIRE(result != -1);
    
    usleep(2500);
    server.stop();
    usleep(2500);
}

TEST_CASE("HTTP Server-Client 400 Bad Request", "[HttpServer]") {
    HttpServer server("tests/libs/httpServer/wwwroot", 8092, 25, 5);
    server.run();
    usleep(2500);

    Connection conn;
    int result = conn.connect("localhost", 8092);
    REQUIRE(result != -1);

    usleep(2500);

    result = conn.Write("GET / HTTP/1.1\r\nHost: localh", 28);
    REQUIRE(result > 0);

    usleep(2500);

    char buff[10*1024];
    result = conn.Read(buff, sizeof(buff));
    REQUIRE(result > 0);

    string responseString(buff, result);
    
    usleep(2500);
    server.stop();
    usleep(2500);
}

TEST_CASE("HTTP Server-Client Endpoint", "[HttpServer]") {
    HttpServer server("tests/libs/httpServer/wwwroot", 8093, 25, 5);
    server.registerEndpoint("/api/test", "GET", endpoint);
    server.run();
    usleep(2500);

    Connection conn;
    int result = conn.connect("localhost", 8093);
    REQUIRE(result == 0);

    result = conn.Write("GET /api/test HTTP/1.1\r\nHost: localhost:8093\r\nConnection: close\r\n\r\n", 59);
    REQUIRE(result > 0);

    char buff[100];
    result = conn.Read(buff, sizeof(buff));
    REQUIRE(result > 0);

    string responseString(buff, result);
    HttpResponse response(responseString);
    REQUIRE(response.isValid);
    
    usleep(2500);
    server.stop();
    usleep(2500);
}

TEST_CASE("HTTP Server-Client Static File", "[HttpServer]") {
    HttpServer server("tests/libs/httpServer/wwwroot", 8094, 25, 5);
    server.run();
    usleep(2500);

    Connection conn;
    int result = conn.connect("localhost", 8094);
    REQUIRE(result != -1);

    result = conn.Write("GET /assets/lobster.jpg HTTP/1.1\r\nHost: localhost:8094\r\nConnection: close\r\n\r\n", 77);
    REQUIRE(result != -1);

    char buff[20*1024];
    result = conn.Read(buff, sizeof(buff));
    REQUIRE(result > 0);

    string responseString(buff, result);
    HttpResponse response(responseString);
    REQUIRE(response.isValid);

    REQUIRE(response.status == HttpResponse::StatusCode::OK);
    REQUIRE(response.headers["Content-Type"] == "image/jpeg");
    
    usleep(2500);
    server.stop();
    usleep(2500);
}

TEST_CASE("HTTP Server-Client 404 NOT FOUND", "[HttpServer]") {
    HttpServer server("tests/libs/httpServer/wwwroot", 8095, 25, 5);
    server.run();
    usleep(2500);

    Connection conn;
    int result = conn.connect("localhost", 8095);
    REQUIRE(result != -1);

    result = conn.Write("GET /test.html HTTP/1.1\r\nHost: localhost:8095\r\nConnection: close\r\n\r\n", 68);
    REQUIRE(result != -1);

    char buff[100];
    result = conn.Read(buff, sizeof(buff));
    REQUIRE(result > 0);

    string responseString(buff, result);
    HttpResponse response(responseString);
    REQUIRE(response.isValid);

    REQUIRE(response.status == HttpResponse::StatusCode::NOT_FOUND);
    
    usleep(2500);
    server.stop();
    usleep(2500);
}

TEST_CASE("HTTP Server-Client Root should be index", "[HttpServer]") {
    HttpServer server("tests/libs/httpServer/wwwroot", 8096, 25, 5);
    server.run();
    usleep(2500);

    Connection conn;
    int result = conn.connect("localhost", 8096);
    REQUIRE(result != -1);

    result = conn.Write("GET / HTTP/1.1\r\nHost: localhost:8096\r\nConnection: close\r\n\r\n", 59);
    REQUIRE(result != -1);

    char buff[20*1024];
    result = conn.Read(buff, sizeof(buff));
    REQUIRE(result > 0);

    string responseString(buff, result);
    HttpResponse response(responseString);
    REQUIRE(response.isValid);

    REQUIRE(response.status == HttpResponse::StatusCode::OK);
    REQUIRE(response.headers["Content-Type"] == "text/html");
    
    usleep(2500);
    server.stop();
    usleep(2500);
}