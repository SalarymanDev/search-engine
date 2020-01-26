#include "src/libs/preprocessing/TermFrequencyProcessor.h"
#include "src/libs/gdfs/gdfs.h"
#include "src/libs/preprocessing/loaders/loaders.h"

using BBG::TermFrequencyProcessor;
using BBG::GDFS;
using BBG::Map;
using BBG::string;
using BBG::loadBow;

TermFrequencyProcessor::TermFrequencyProcessor(vector<Document*> &docs, const char *filename, Map<string, uint32_t>& bow)
    : docs(docs), bow(bow) {
    logger.Info("Loading TF from file ...");
    if (loadTF(filename, TF)) return;
    logger.Info("Calculating TF ...");
    calcTF();
    logger.Info("Calculating TF complete");
    logger.Info("Writing TF to disk ...");
    write_data(TF, filename);
    logger.Info("Writing TF to disk complete");
}

Map<uint32_t, Map<string, uint32_t>>& TermFrequencyProcessor::get() {
    return TF;
}

void TermFrequencyProcessor::updateTF(Document* doc) {
    for (size_t j = 0; j < doc->titleText.size(); ++j)
        TF[bow[doc->titleText[j]]][doc->url] += 1;

    for (size_t j = 0; j < doc->bodyText.size(); ++j)
        TF[bow[doc->bodyText[j]]][doc->url] += 1;
}


void TermFrequencyProcessor::calcTF() {
    for (size_t i = 0; i < docs.size(); ++i)
        updateTF(docs[i]);
}
