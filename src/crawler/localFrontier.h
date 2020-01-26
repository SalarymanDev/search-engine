#pragma once
#include <pthread.h>

#include "src/crawler/frontierClient.h"
#include "src/libs/document/Document.h"
#include "src/libs/queue/queue.h"
#include "src/libs/string/string.h"

namespace BBG {
class LocalFrontier {
  // Consts
  constexpr static size_t halfMaxQueueCapacity = 1;
  constexpr static size_t fetchMax = 500;

  // Sync
  pthread_cond_t workerCV_ = PTHREAD_COND_INITIALIZER;
  pthread_cond_t refillQueueCV_ = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t urlLock_ = PTHREAD_MUTEX_INITIALIZER;

  // Queue of urls to give to worker threads
  queue<string>* urls_;

  // Client to global frontier
  FrontierClient* fc_;

  // Waits on a signal for refillQueueCV for queue size to be less than
  // half the desired size
  static void* queueFiller(void*);
  pthread_t queueFillingThread_;

 public:
  LocalFrontier(const string& frontierUrl) {
    fc_ = new FrontierClient(frontierUrl);
    urls_ = new queue<string>();
    pthread_create(&queueFillingThread_, NULL, &queueFiller, this);
  };

  ~LocalFrontier() {
    // delete fc_;
    // delete urls_;
  }

  // Wait for a url to crawl
  string WaitForWork();

  void AddUrls(Document&);
  void MarkCrawled(const vector<string>&);
};
}  // namespace BBG
