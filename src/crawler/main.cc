#include <cstdlib>
#include "src/crawler/crawler.h"
#include "src/libs/logger/logger.h"

#include <signal.h>

using BBG::Crawler;
using BBG::Logger;
using BBG::string;

int main() {
  Logger logger;
  signal(SIGPIPE, SIG_IGN);

  char* furl = getenv("BBGFRONTIER");
  if (!furl) {
    logger.Fatal("[Crawler]\tNo frontier url specified");
  }
  string furls(furl);

  Crawler c(furls);
  logger.Info("Starting crawler...");
  c.Start();
}
