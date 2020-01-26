#include <unistd.h>
#include <iostream>
#include "src/libs/rpc/RpcClient.h"
#include "src/libs/rpc/RpcServer.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/string/string.h"
#include "src/libs/utils/utils.h"
#include "src/libs/vector/vector.h"
#include "tests/catch.hpp"

using BBG::Buffer;
using BBG::RpcClient;
using BBG::RpcServer;
using BBG::string;
using BBG::strncmp;
using BBG::vector;
using std::cout;
using std::endl;

Buffer testEndpoint(Buffer& request) {
  cout << request.data() << endl;
  return Buffer("Test Response", 14);
}

TEST_CASE("Client Constructor", "[RpcClient]") {
  RpcClient client("https://google.com");
}

TEST_CASE("Server Constructor with port binding", "[RpcServer]") {
  RpcServer server(8080);
  server.registerEndpoint("test", testEndpoint);
  server.run();
  usleep(5000);
  server.stop();
  usleep(5000);
}

TEST_CASE("Rpc Server-Client Blocking Communication",
          "[RpcServer, RpcClient]") {
  RpcServer server(8081);
  server.registerEndpoint("test", testEndpoint);
  server.run();
  usleep(5000);
  RpcClient* client = new RpcClient("rpc:localhost:8081");
  Buffer requestBytes("Test Request", 13);
  Buffer responseBytes = client->request("test", requestBytes);
  usleep(5000);
  REQUIRE(!strncmp("Error:", responseBytes.data(), 6));
  delete client;
  usleep(5000);
  server.stop();
  usleep(5000);
}

TEST_CASE("Rpc Server-Client NonBlocking Communication",
          "[RpcServer, RpcClient]") {
  RpcServer server(8082);
  server.registerEndpoint("test", testEndpoint);
  server.run();
  usleep(5000);
  RpcClient* client = new RpcClient("rpc:localhost:8082");
  Buffer requestBytes("Test Request", 13);
  client->requestNonBlocking("test", requestBytes);
  usleep(5000);
  delete client;
  usleep(5000);
  server.stop();
  usleep(5000);
}

TEST_CASE("Rpc Server Should return error response", "[RpcServer]") {
  RpcServer server(8083);
  server.run();
  usleep(5000);
  RpcClient* client = new RpcClient("rpc:localhost:8083");
  Buffer result = client->request("test", Buffer("", 1));
  REQUIRE(strncmp("Error:", result.data(), 6));
  usleep(5000);
}

Buffer check_is_5(Buffer& buff) {
  size_t maybe_5;
  deserialize(buff, maybe_5);
  REQUIRE(maybe_5 == 500);

  maybe_5 = 1000;
  Buffer resp;
  serialize(maybe_5, resp);
  return resp;
}

TEST_CASE("Test ints", "[RpcServer]") {
  RpcServer server(8084);
  server.registerEndpoint("is_5", check_is_5);
  server.run();
  usleep(5000);

  RpcClient* client = new RpcClient("rpc:localhost:8084");
  Buffer req;
  size_t five = 500;
  serialize(five, req);

  Buffer resp = client->request("is_5", req);
  deserialize(resp, five);
  REQUIRE(five == 1000);
  delete client;
  usleep(5000);
  server.stop();
  usleep(5000);
}

Buffer give_urls(Buffer& bytes) {
  size_t num_urls;
  deserialize(bytes, num_urls);
  vector<string> urls(num_urls);
  printf("Got req for %zu urls\n", num_urls);
  for (size_t i = 0; i < num_urls; i++) {
    printf("Pushing\n");
    urls[i] = "http://example.com";
  }
  bytes.clear();
  REQUIRE(urls.size() == num_urls);
  serialize(urls, bytes);
  return bytes;
}

TEST_CASE("Test urls repeated", "[RpcServer]") {
  RpcServer server(8085);
  server.registerEndpoint("give_urls", give_urls);
  server.run();
  usleep(5000);

  RpcClient* client = new RpcClient("rpc:localhost:8085");
  Buffer req;
  size_t five = 2;
  serialize(five, req);

  // Make the call twice to ensure rpc server handles multiple calls correctly
  Buffer resp = client->request("give_urls", req);
  for (size_t i = 50; i--; client->request("give_urls", req)) {
  }

  vector<string> urls;
  deserialize(resp, urls);
  REQUIRE(urls.size() == five);
  for (size_t i = 0; i < five; i++) REQUIRE(urls[i] == "http://example.com");

  delete client;
  usleep(5000);
  server.stop();
  usleep(5000);
}
