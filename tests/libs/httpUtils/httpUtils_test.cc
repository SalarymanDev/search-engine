#include "src/libs/httpUtils/httpUtils.h"
#include "src/libs/fnv/fnv.h"
#include "src/libs/string/string.h"
#include "tests/catch.hpp"

#include <openssl/ssl.h>

#include <sys/mman.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <thread>

void startHTTPserver() {
  system("httpserver");
  // For debug
  // system("bazel-bin/tests/httpserver/linux_amd64_debug/httpserver");
}

TEST_CASE("Crawling works on simple pages") {
  // Start the http server & wait for it to boot
  std::thread serverThread(startHTTPserver);
  sleep(2);

  BBG::string outputString;
  BBG::vector<BBG::string> urls;
  BBG::HttpUtil u;
  BBG::HttpUtil::ReturnCode ret;

  SECTION("Basic") {
    urls.push_back("http://localhost:8000/index.html");
    ret = u.GetPage(urls, outputString);
    REQUIRE(ret == BBG::HttpUtil::Ok);
    BBG::string expect("<html><body><h1>Testing page</h1></body></html>");
    REQUIRE(outputString == expect);
  }

  SECTION("Empty Page") {
    urls.push_back("http://localhost:8000/empty");
    ret = u.GetPage(urls, outputString);
    REQUIRE(ret == BBG::HttpUtil::Ok);
  }

  SECTION("Handles Redirects") {
    urls.push_back("http://localhost:8000/redirect/301");
    ret = u.GetPage(urls, outputString);
    REQUIRE(ret == BBG::HttpUtil::Ok);
    BBG::string expect("Redirected 301");
    REQUIRE(outputString == expect);
  }

  SECTION("Handles Redirect loops") {
    urls.push_back("http://localhost:8000/redirect/loop");
    ret = u.GetPage(urls, outputString);
    REQUIRE(ret == BBG::HttpUtil::Redirected);

    for (size_t i = 0; i < urls.size(); i++) REQUIRE(urls[i] == urls[0]);
  }

  SECTION("Handles HTTPS") {
    urls.push_back("https://example.com");
    SSL_CTX* ctx = SSL_CTX_new(SSLv23_method());

    ret = u.GetPage(urls, outputString);
    REQUIRE(ret == BBG::HttpUtil::Ok);
    uint64_t hash = BBG::fnv(outputString.c_str(), outputString.size());
    REQUIRE(hash == 6371474564111668196);
    SSL_CTX_free(ctx);
  }

  SECTION("Discards application/json") {
    urls.push_back("http://localhost:8000/json");
    ret = u.GetPage(urls, outputString);
    REQUIRE(ret == BBG::HttpUtil::MalformedHeader);
    REQUIRE(outputString.empty());
  }

  // Shutdown the http server by hitting the endpoint with the crawler
  urls.push_back("http://localhost:8000/shutdown");
  u.GetPage(urls, outputString);
  serverThread.join();
}
