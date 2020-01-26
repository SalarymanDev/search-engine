#include <dirent.h>
#include <iostream>
#include "src/libs/httpServer/HttpServer.h"
#include "src/libs/logger/logger.h"
#include "src/libs/map/map.h"
#include "src/libs/preprocessing/loaders/loaders.h"
#include "src/libs/rank/ranker/ranker.h"
#include "src/libs/rank/PriorityQueue/PriorityQueue.h"
#include "src/libs/rpc/RpcClient.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "unistd.h"

using namespace BBG;

size_t NUM_URLS = 10;

Map<string, uint32_t> bagOfWords;
vector<string> indexFileNames;
Logger loggy;

HttpResponse *dummyQueryEndpoint(HttpRequest &request) {
  HttpResponse *response = new HttpResponse();
  response->status = HttpResponse::StatusCode::OK;
  response->headers["Content-Type"] = "application/json";
  // CORS Header for testing
  response->headers["Access-Control-Allow-Origin"] = "*";
  response->body.append(
      "[\"https://nicolehamilton.com/\", \"https://umich.edu/\", "
      "\"https://github.com/\", \"https://en.wikipedia.org/\", "
      "\"https://youtube.com\", \"http://listen.hatnote.com/\", "
      "\"https://piazza.com/\", "
      "\"https://wolverineaccess.umich.edu/f/u24l1s13/normal/render.uP\", "
      "\"https://umich.instructure.com/\", \"https://www.gradescope.com/\"]",
      311);
  return response;
}

HttpResponse *queryEndpoint(HttpRequest &request) {
  HttpResponse *response = new HttpResponse();
  response->status = HttpResponse::StatusCode::OK;
  response->headers["Content-Type"] = "application/json";

  vector<QueryResult> results;
  if (!request.body.empty())
    results = getBestUrls(request.body, NUM_URLS, bagOfWords, indexFileNames);

  // CORS Header for testing
  response->headers["Access-Control-Allow-Origin"] = "*";

  // Format repsonse
  response->body.append("[", 1);

  // No results found
  if (results.empty()) {
    response->body = "{\"title\":\"Try Google?\", \"url\": \"https://google.com/\"}]";
    loggy.Debug(response->body);
    return response;
  }

  for (size_t i = 0; i < results.size() - 1; ++i) {
    response->body.append("{\"title\":\"", 10);
    if (!results[i].title.empty())
      response->body.append(results[i].title.c_str(), results[i].title.size());
    else
      response->body.append(results[i].url.c_str(), results[i].url.size());

    response->body.append("\",\"url\":\"", 9);
    response->body.append(results[i].url.c_str(), results[i].url.size());
    response->body.append("\"},", 3);
  }
  response->body.append("{\"title\":\"", 10);
  if (!results.back().title.empty())
    response->body.append(results.back().title.c_str(), results.back().title.size());
  else
    response->body.append(results.back().url.c_str(), results.back().url.size());
  response->body.append("\",\"url\":\"", 9);
  response->body.append(results.back().url.c_str(), results.back().url.size());
  response->body.append("\"}]", 3);

  // For debugging
  loggy.Debug(response->body);
  return response;
}

void getIndexFilenames(const string &dirName, vector<string> &filenames) {
  DIR *dir = opendir(dirName.c_str());
  if (!dir) loggy.Fatal("Unable to open directory");

  filenames.clear();

  dirent *entry;
  while ((entry = readdir(dir))) {
    if (entry->d_type != DT_REG && entry->d_type != DT_LNK) continue;

    filenames.push_back(dirName + "/" + entry->d_name);
  }
}

int main(int argc, char **argv) {
  loadBow("BOW", bagOfWords);
  getIndexFilenames("index", indexFileNames);
  HttpServer server("wwwroot", 80, SOMAXCONN);
  server.registerEndpoint("/query", "POST", queryEndpoint);
  server.run();
  server.join();
  return 0;
}
