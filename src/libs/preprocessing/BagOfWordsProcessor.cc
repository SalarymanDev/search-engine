#include "src/libs/preprocessing/BagOfWordsProcessor.h"
#include "src/libs/gdfs/gdfs.h"
#include "src/libs/preprocessing/loaders/loaders.h"

using BBG::BagOfWordsProcessor;
using BBG::GDFS;
using BBG::vector;
using BBG::string;
using BBG::loadBow;
using BBG::Map;

BagOfWordsProcessor::BagOfWordsProcessor(vector<Document*> &docs, const char* filename) : docs(docs) {
    logger.Info("Loading BOW from file ...");
    if (loadBow(filename, bow)) return;
    logger.Info("Calculating BOW ...");
    calcBow();
    logger.Info("Calculating BOW complete");
    logger.Info("Writing BOW to disk ...");
    write_data(bow, filename);
    logger.Info("Writing BOW to disk complete");
}

Map<string, uint32_t>& BagOfWordsProcessor::get() {
    return bow;
}

void BagOfWordsProcessor::addToBow(const vector<string>& words) {
    for (size_t i = 0; i < words.size(); ++i) {
        if (!bow.contains(words[i]))
            bow[words[i]] = word_id++;
    }
}

// Loops through document set and eveytime it encounters a new word
// it adds it to the bow map with an increasing id
void BagOfWordsProcessor::addDocToBow(Document* doc) {
    // logger.Info("Adding " + doc->url + " to the bag of words...");
    addToBow(doc->titleText);
    addToBow(doc->bodyText);
    for (auto elem = doc->anchors.begin(); elem; elem = doc->anchors.next())
        addToBow(elem->val);
}

void BagOfWordsProcessor::calcBow() {
    for (size_t i = 0; i < docs.size(); ++i) {
        // logger.Info("Adding doc to bow: " + to_string((unsigned long)docs[i]));
        addDocToBow(docs[i]);
    }
}
