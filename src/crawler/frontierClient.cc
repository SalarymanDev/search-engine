#pragma once
#include <stdlib.h>

#include "src/crawler/frontierClient.h"
#include "src/libs/logger/logger.h"
#include "src/libs/queue/queue.h"
#include "src/libs/rpc/RpcClient.h"
#include "src/libs/serialize/Buffer.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/string/string.h"
#include "src/libs/utils/utils.h"
#include "src/libs/vector/vector.h"

using namespace BBG;

void FrontierClient::GetUrls(size_t desiredUrls, queue<string>& urls) {
  Buffer req;
  vector<string> respUrls;

  serialize(desiredUrls, req);
  pthread_mutex_lock(&cliMu_);
  Buffer resp = cli_->request("recv_urls", req);
  pthread_mutex_unlock(&cliMu_);
  deserialize(resp, respUrls);

  for (size_t i = 0; i < respUrls.size(); i++) {
    if (respUrls[i].size() > 0 && respUrls[i][0] == 'h') {
      urls.push(respUrls[i]);
    }
  }
}

void FrontierClient::AddUrls(const vector<string>& urls) {
  char size[20];
  sprintf(size, "%zu", urls.size());
  logger_.Debug("[FrontierClient:AddUrls] sending " + string(size) + " urls to frontier");
  Buffer req;
  serialize(urls, req);
  pthread_mutex_lock(&cliMu_);
  cli_->requestNonBlocking("send_urls", req);
  pthread_mutex_unlock(&cliMu_);
}

void FrontierClient::MarkCrawled(const vector<string>& urls) {
  Buffer req;

  serialize(urls, req);
  pthread_mutex_lock(&cliMu_);
  cli_->requestNonBlocking("mark_crawled", req);
  pthread_mutex_unlock(&cliMu_);
}
