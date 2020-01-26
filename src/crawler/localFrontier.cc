#include "src/crawler/localFrontier.h"

#include <pthread.h>
#include <unistd.h>

#include "src/libs/logger/logger.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"

string LocalFrontier::WaitForWork() {
  pthread_mutex_lock(&urlLock_);

  // If there are no urls left in the queue, block
  while (urls_->size() == 0) {
    pthread_cond_signal(&refillQueueCV_);
    pthread_cond_wait(&workerCV_, &urlLock_);
  }

  // Get the first url
  string ret = urls_->front();
  Logger log;
  log.Debug(ret);
  urls_->pop();

  // Signal if we need to refill
  if (urls_->size() < halfMaxQueueCapacity)
    pthread_cond_signal(&refillQueueCV_);

  pthread_mutex_unlock(&urlLock_);
  return ret;
}

void* LocalFrontier::queueFiller(void* args) {
  LocalFrontier* self = (LocalFrontier*)args;
  while (true) {
    pthread_mutex_lock(&self->urlLock_);

    // Wait until the queue is half empty
    while (self->urls_->size() > self->halfMaxQueueCapacity)
      pthread_cond_wait(&self->refillQueueCV_, &self->urlLock_);

    // Fill with urls from the frontier
    // XXX: We are holding the lock while waiting on the network here, we could
    // prefetch the urls
    self->fc_->GetUrls(self->fetchMax, *self->urls_);

    pthread_mutex_unlock(&self->urlLock_);

    // Signal any workers that stopped because the queue emptied
    pthread_cond_broadcast(&self->workerCV_);

    usleep(1000);
  }
}

void LocalFrontier::AddUrls(Document& doc) {
  // TODO: Batching
  fc_->AddUrls(doc.getFilteredUrls());
}

void LocalFrontier::MarkCrawled(const vector<string>& urls) {
  // TODO: Batching
  fc_->MarkCrawled(urls);
}
