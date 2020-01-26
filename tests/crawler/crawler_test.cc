#include "src/crawler/crawler.h"
#include "src/libs/rpc/RpcServer.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/string/string.h"
#include "src/libs/thread/lockguard.h"
#include "src/libs/vector/vector.h"
#include "tests/catch.hpp"

#include <unistd.h>

using namespace BBG;

Logger logger;
bool running = true;
pthread_mutex_t request_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t request_cv = PTHREAD_COND_INITIALIZER;

Buffer send_urls(Buffer &bytes) {
  lockguard lg(&request_mutex);
  // Deserialize request
  size_t num_urls;
  deserialize(bytes, num_urls);

  char line[1024];
  sprintf(line, "Got request for %d urls", num_urls);
  string s(line);
  logger.Debug(s);

  // Handle request
  vector<string> urls(num_urls);
  for (size_t i = 0; i < num_urls; i++) {
    logger.Debug("Sending");
    urls[i] = "http://example.com";
  }

  // Serialize response
  bytes.clear();
  serialize(urls, bytes);
  return bytes;
}

Buffer recv_urls(Buffer &bytes) {
  lockguard lg(&request_mutex);
  logger.Debug("Adding Urls");

  vector<string> urls;
  deserialize(bytes, urls);

  for (size_t i = 0; i < urls.size(); i++) {
    REQUIRE(urls[i] == "https://www.iana.org/domains/example");
  }

  return Buffer();
}

Buffer mark_crawled(Buffer &bytes) {
  lockguard lg(&request_mutex);
  vector<string> urls;
  deserialize(bytes, urls);

  logger.Debug("Marked Urls");
  for (size_t i = 0; i < urls.size(); i++) {
    // REQUIRE(urls[i] == "http://example.com");
  }

  return Buffer();
}

Buffer shutdown(Buffer &bytes) {
  lockguard lg(&request_mutex);
  running = false;
  pthread_cond_signal(&request_cv);
  return Buffer();
}

void setupEnvironment() {
    putenv("BBG_STOPWORDS=src/libs/naughtyFilter/stopwords.txt");
    REQUIRE(getenv("BBG_STOPWORDS"));
    putenv("BBG_BLACKLIST=src/libs/naughtyFilter/blacklist.txt");
    REQUIRE(getenv("BBG_BLACKLIST"));
}

TEST_CASE("Crawling", "[Crawler]") {
  setupEnvironment();
  RpcServer server(8080, 200);
  logger.Debug("Registering endpoints...");
  server.registerEndpoint("recv_urls", &send_urls);
  server.registerEndpoint("send_urls", &recv_urls);
  server.registerEndpoint("mark_crawled", &mark_crawled);
  server.registerEndpoint("shutdown", &shutdown);
  server.run();

  logger.Debug("Starting crawler...");
  Crawler c("rpc:localhost:8080");
  usleep(2000);
  c.Start();

  lockguard lg(&request_mutex);
  while (running) pthread_cond_wait(&request_cv, &request_mutex);

  server.stop();
}
