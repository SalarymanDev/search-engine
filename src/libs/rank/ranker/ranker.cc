#include "src/libs/rank/ranker/ranker.h"
#include "src/libs/logger/logger.h"

using BBG::string;

// BBG::Logger llogy;

float BBG::get_link_weight(size_t link_count) {
    if (link_count > 400)
        return 0.2;
    else if (link_count > 300)
        return 0.15;
    else if (link_count > 200)
        return 0.1;
    else if (link_count > 100)
        return 0.05;
    return 0.0;
}

float BBG::getStaticRank(DocumentMatch& docMatch) {
    /*
        Static features:
            number of slashes in url
            length of url
            protocol
            extension
            document length (docend.features.length)
            length of title
    */
    
    string url = docMatch.docEnd->getUrl();
    
    float avg_num_slashes = 6 + 2;
    float avg_url_length = 77;
    // float avg_word_length = 4.7;
    
    float num_slashes = 0.3 * (url.contains('/') / avg_num_slashes);

    float url_length = 0.05 * url.size() / avg_url_length;

    float protocol = 0.2 * protocolIsHttps(url);

    float extension = 0.2 * extensionIsGood(url);

    float incoming_links = get_link_weight(docMatch.docEnd->docFeatures.getIncomingLinkCount());

    float outgoing_links = get_link_weight(docMatch.docEnd->docFeatures.getOutgoingLinkCount());
    
    float img_count = docMatch.docEnd->docFeatures.getImgCount( ) > 8 ? 0.05 : 0;

    float doc_len_tmp = docMatch.docEnd->docFeatures.numWords;
    float doc_length = 0.2 * (doc_len_tmp >= 200 && doc_len_tmp <= 5000);

    float num_title_words = docMatch.docEnd->getTitle().contains(' ') + 1;
    float title_length = 0.05 * (num_title_words >= 2 && num_title_words <= 10);

    return num_slashes + url_length + protocol + extension 
        + incoming_links + outgoing_links + img_count +  doc_length + title_length;
}

float BBG::getDynamicRank(DocumentMatch& docMatch) {
    vector<float> queryVec;
    vector<float> docVec;
    for (size_t i = 0; i < docMatch.wordFeatures.size(); ++i) {
        queryVec.push_back(docMatch.wordFeatures[i]->idf); // assumming only 1 occurence of each word
        docVec.push_back(docMatch.wordDocFeatures[i]->getTermFreq() * docMatch.wordFeatures[i]->idf);
    }

    float tfidf = dotprod(queryVec, docVec);
    tfidf /= l2norm(queryVec) * docMatch.docEnd->docFeatures.docNorm;

    float anchorCount = 0.0;
    for (size_t i = 0; i < docMatch.wordDocFeatures.size(); ++i)
        anchorCount += docMatch.wordDocFeatures[i]->getAnchorTermCount();

    float avgAnchorCount = log10(200);
    anchorCount /= avgAnchorCount;

    float avgTextType = mean(docMatch.htmlTags);

    float dynamicRank = 0.25 * tfidf + 0.50 * anchorCount + 0.25 * avgTextType;

    return dynamicRank;
}

float BBG::getScore(DocumentMatch& docMatch) {
    float static_score = getStaticRank(docMatch);
    float dynamic_score = getDynamicRank(docMatch);
    return 0.5 * static_score + 0.5 * dynamic_score;
}

void BBG::mergeTopNs(vector<PriorityQueue> &topNs, vector<QueryResult> &output) {
    while (output.size() < output.capacity()) {
        size_t bestIndex = 0;
        float bestScore = -1.0;

        for (size_t j = 0; j < topNs.size(); ++j) {
            if (topNs[j].empty())
                continue;

            float topScore = topNs[j].topScore();

            if (topScore > bestScore) {
                bestScore = topScore;
                bestIndex = j;            }
        }

        if (compareFloat(bestScore, -1.0))
            break;

        // llogy.Error("[mergeTopNs] before pushback to output from topNs[" + to_string(bestIndex) + "]");
        QueryResult result = topNs[bestIndex].popBottom();
        bool isDuplicate = false;
        for (size_t i = 0; i < output.size(); ++i) {
            if (result.url == output[i].url) {
                isDuplicate = true;
                break;
            }
        }
        if (!isDuplicate)
            output.push_back(result);
        // llogy.Error("[mergeTopNs] after pushback to output");
    }
}

void * BBG::parallel_query(void *arg) {

    QueryArgs* args = static_cast<QueryArgs*>(arg);
    const char* indexChunkFname = args->indexChunkFname;
    const string& request = *args->request;
    PriorityQueue& topN = *args->topN;
    const Map<string, uint32_t> *bagOfWords = args->bagOfWords;
    delete args;

    IndexFileReader ifr(indexChunkFname);

    IndexChunk* idxChunk = ifr.Load();

    // if (idxChunk->MaximumLocation > (1 << 30))
    // 	llogy.Info("[BBG::parallel_query] Index chunk too many words");

    QueryCompiler qCompiler(idxChunk, bagOfWords);

    ISR* isr = qCompiler.parseQuery(request);

    if (!isr)
        return nullptr;

    DocumentMatch docMatch;
    QueryResult qResult;

    while ((docMatch = isr->NextDocument()).docEnd != nullptr) {
        qResult.url = docMatch.docEnd->getUrl();
        qResult.title = docMatch.docEnd->getTitle();
        topN.push(qResult, getScore(docMatch));
    }

    delete isr;

    return nullptr;
}

vector<QueryResult> BBG::getBestUrls(string& request, size_t numUrls, 
                                const Map<string, uint32_t>& bagOfWords,
                                const vector<string>& indexFileNames) {
    size_t numIndexChunks = indexFileNames.size();

    vector<PriorityQueue> topNs(numIndexChunks);
    vector<pthread_t> threads;
    threads.reserve(numIndexChunks);
    for (size_t i = 0; i < numIndexChunks; ++i) {
        QueryArgs *arg = new QueryArgs{&topNs[i], 
                                       &bagOfWords, 
                                       indexFileNames[i].c_str(),
                                       &request};
        pthread_t thread;
        pthread_create(&thread, nullptr, parallel_query, static_cast<void*>(arg));
        threads.push_back(thread);
    }

    for (size_t i = 0; i < numIndexChunks; ++i) {
        pthread_join(threads[i], nullptr);
    }

    // llogy.Error("[getBestUrls] after thread join");

    vector<QueryResult> results;
    results.reserve(numUrls);
    mergeTopNs(topNs, results);
    return results;
}

vector<QueryResult> BBG::getBestUrlsNaive(string& request, size_t numUrls, 
                                     const Map<string, uint32_t>& bagOfWords,
                                     const vector<string>& indexFileNames) {
    vector<QueryResult> results;
    results.reserve(numUrls);

    for (size_t i = 0; i < indexFileNames.size(); ++i) {
        if (results.size() == numUrls)
            break;
        
        IndexFileReader ifr(indexFileNames[i].c_str());
        IndexChunk * idxChunk = ifr.Load();

        QueryCompiler qCompiler(idxChunk, &bagOfWords);
        ISR* isr = qCompiler.parseQuery(request);

        if (!isr)
            return results;

        DocumentMatch docMatch;
        QueryResult qResult;

        while ((docMatch = isr->NextDocument()).docEnd != nullptr) {
            if (results.size() == numUrls)
                break;
            qResult.title = docMatch.docEnd->getTitle();
            qResult.url = docMatch.docEnd->getUrl();
            results.push_back(qResult);
        }
    }
    return results;
}
