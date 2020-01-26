#ifndef RANKER_H
#define RANKER_H

#include "src/libs/rank/PriorityQueue/PriorityQueue.h"
#include "src/libs/rank/rank.h"
#include "src/libs/frontier/rank/rank.h"
#include "src/libs/queryCompiler/queryCompiler.h"
#include "src/libs/preprocessing/loaders/loaders.h"

namespace BBG {

struct QueryArgs {
    PriorityQueue* topN;
    const Map<string, uint32_t> *bagOfWords;
    const char* indexChunkFname;
    const string *request;


    QueryArgs(PriorityQueue* _topN, const Map<string, uint32_t> *_bagOfWords, 
              const char* _indexChunkFname, const string *_request)
        : topN(_topN), bagOfWords(_bagOfWords), 
          indexChunkFname(_indexChunkFname), request(_request) {}
};

float get_link_weight(size_t link_count);
float getStaticRank(DocumentMatch& docMatch);
float getDynamicRank(DocumentMatch& docMatch);
float getScore(DocumentMatch& docMatch);

void mergeTopNs(vector<PriorityQueue> &topNs, vector<QueryResult> &output);
void * parallel_query(void *arg);

vector<QueryResult> getBestUrls(string& request, size_t numUrls, 
                                const Map<string, uint32_t>& bagOfWords,
                                const vector<string>& indexFileNames);
vector<QueryResult> getBestUrlsNaive(string& request, size_t numUrls, 
                                     const Map<string, uint32_t>& bagOfWords,
                                     const vector<string>& indexFileNames);
}

#endif
