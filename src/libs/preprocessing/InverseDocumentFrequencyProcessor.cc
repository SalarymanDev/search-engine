#include "src/libs/preprocessing/InverseDocumentFrequencyProcessor.h"
#include "src/libs/utils/utils.h"
#include "src/libs/preprocessing/loaders/loaders.h"
#include <math.h>

using BBG::InverseDocumentFrequencyProcessor;
using BBG::write_data;
using BBG::loadTF;
using BBG::loadBow;
using BBG::Map;

InverseDocumentFrequencyProcessor::InverseDocumentFrequencyProcessor(const char *filename, Map<string, uint32_t>& bow, Map<uint32_t, Map<string, uint32_t>>& tf) : BOW(bow), TF(tf) {
    logger.Info("Loading IDF from file ...");
    if (loadIDF(filename, IDF)) return;
    logger.Info("Calculating IDF ...");
    calcIDF();
    logger.Info("Calculating IDF complete");
    logger.Info("Writing IDF to disk ...");
    write_data(IDF, filename);
    logger.Info("Writing IDF to disk complete");
}

Map<uint32_t, float>& InverseDocumentFrequencyProcessor::get() {
    return IDF;
}

void InverseDocumentFrequencyProcessor::makeNK() {
    for (auto elem = BOW.begin(); elem; elem = BOW.next())
        NK[elem->val] = TF[elem->val].size();
}


void InverseDocumentFrequencyProcessor::updateIDF() {
    for (auto elem = BOW.begin(); elem; elem = BOW.next())
        IDF[elem->val] = (float) log10(N/(float) NK[elem->val]);
}


void InverseDocumentFrequencyProcessor::calcIDF() {
    makeNK();
    updateIDF();
}
