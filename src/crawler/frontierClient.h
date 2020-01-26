#pragma once
#include <stdlib.h>

#include "src/libs/queue/queue.h"
#include "src/libs/rpc/RpcClient.h"
#include "src/libs/serialize/Buffer.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/string/string.h"
#include "src/libs/utils/utils.h"
#include "src/libs/vector/vector.h"

using namespace BBG;

class FrontierClient {
  RpcClient* cli_;
  pthread_mutex_t cliMu_ = PTHREAD_MUTEX_INITIALIZER;
  Logger logger_;

 public:
  // Connect to the frontier at envvar BBGFRONTIER
  int Init();
  FrontierClient(const string& furl) : cli_(new RpcClient(furl)) {}
  ~FrontierClient() { delete cli_; }

  void GetUrls(size_t, queue<string>&);
  void AddUrls(const vector<string>&);
  void MarkCrawled(const vector<string>&);
};
