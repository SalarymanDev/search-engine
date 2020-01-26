#include <pthread.h>

#include "src/crawler/crawler.h"
#include "src/libs/document/Document.h"
#include "src/libs/htmlParser/HtmlParser.h"
#include "src/libs/httpUtils/httpUtils.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/logger/logger.h"

LocalFrontier* BBG::Crawler::lf_;
GDFS* BBG::Crawler::gdfs_;
const NaughtyFilter* BBG::Crawler::stopwordsFilter;

void* BBG::Crawler::worker(void*) {
  HttpUtil h;
  string body;

  HtmlParser p(*stopwordsFilter);
  vector<string> urls;
  Document doc;

  string logline;
  char c[9];
  Logger log;

  while (true) {
    body.clear();
    urls.clear();
    doc.clear();

    urls.push_back(lf_->WaitForWork());

    HttpUtil::ReturnCode ret = h.GetPage(urls, body);
    switch (ret) {
      case HttpUtil::SystemError:
      case HttpUtil::MalformedHeader:
      case HttpUtil::BadResponse:
        break;

      case HttpUtil::Redirected:
        lf_->MarkCrawled(urls);
        break;

      case HttpUtil::Ok:
        lf_->MarkCrawled(urls);
        doc.url = urls.back();
        p.parse(body, doc);
        lf_->AddUrls(doc);
        gdfs_->addDocument(doc);
        break;
    }
  }
}

void BBG::Crawler::Start() {
  vector<pthread_t> threads(numThreads);

  for (size_t i = 0; i < numThreads; i++)
    pthread_create(&threads[i], NULL, &BBG::Crawler::worker, NULL);

  // Wait for threads that never exit to exit, lol
  for (size_t i = 0; i < numThreads; i++) pthread_join(threads[i], nullptr);
}
