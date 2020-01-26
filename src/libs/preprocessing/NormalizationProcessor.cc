#include "src/libs/preprocessing/NormalizationProcessor.h"
#include "src/libs/gdfs/gdfs.h"
#include "src/libs/preprocessing/loaders/loaders.h"
#include <math.h>

using BBG::NormalizationProcessor;
using BBG::GDFS;
using BBG::Map;
using BBG::string;

NormalizationProcessor::NormalizationProcessor(vector<Document*> &docs, const char *filename, Map<string, uint32_t>& bow, Map<uint32_t, Map<string, uint32_t>>& tf, Map<uint32_t, float>& idf)
    : docs(docs), BOW(bow), TF(tf), IDF(idf) {
    logger.Info("Loading NORM from file ...");
    if (loadNorm(filename, NORM)) return;
    logger.Info("Calculating NORM ...");
    calcNorm();
    logger.Info("Calculating NORM complete");
    logger.Info("Writing NORM to disk ...");
    write_data(NORM, filename);
    logger.Info("Writing NORM to disk complete");
}

Map<string, float>& NormalizationProcessor::get() {
    return NORM;
}

void NormalizationProcessor::updateNorm(Document& doc) {
    float _sum = 0;

    for (size_t j = 0; j < doc.titleText.size(); ++j) {
        uint32_t & term = BOW[doc.titleText[j]];
        _sum += TF[term][doc.url] * TF[term][doc.url] + IDF[term] * IDF[term];
    }

    for (size_t j = 0; j < doc.bodyText.size(); ++j) {
        uint32_t & term = BOW[doc.bodyText[j]];
        _sum += TF[term][doc.url] * TF[term][doc.url] + IDF[term] * IDF[term];
    }
    
    NORM[doc.url] = (float)sqrt(_sum);
}

void NormalizationProcessor::calcNorm() {
    for (size_t i = 0; i < docs.size(); ++i)
        updateNorm(*docs[i]);
}
