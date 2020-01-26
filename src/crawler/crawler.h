#pragma once
#include <openssl/ssl.h>

#include "src/crawler/localFrontier.h"
#include "src/libs/naughtyFilter/NaughtyFilter.h"
#include "src/libs/gdfs/gdfs.h"
#include "src/libs/queue/queue.h"
#include "src/libs/string/string.h"

namespace BBG {
class Crawler {
  // Consts
  constexpr static size_t numThreads = 50;

  // Local frontier of urls to crawl
  static LocalFrontier* lf_;

  // File manager to store parsed documents
  static GDFS* gdfs_;

  // Function that is run in all threads
  static void* worker(void*);

  // Shared stop words filter
  static const NaughtyFilter* stopwordsFilter;

 public:
  Crawler(const string& frontierUrl) {
    lf_ = new LocalFrontier(frontierUrl);
    gdfs_ = new GDFS();
    stopwordsFilter = new NaughtyFilter(true);
  }
  ~Crawler() {
    delete lf_;
    delete gdfs_;
    delete stopwordsFilter;
  };

  void Start();
};

}  // namespace BBG
