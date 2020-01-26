#ifndef BAG_OF_WORDS_PROCESSOR_H
#define BAG_OF_WORDS_PROCESSOR_H

#include "src/libs/logger/logger.h"
#include "src/libs/map/map.h"
#include "src/libs/string/string.h"
#include "src/libs/document/Document.h"
#include "src/libs/vector/vector.h"

namespace BBG {
    class BagOfWordsProcessor {
    public:
        BagOfWordsProcessor(vector<Document*> &docs, const char* filename);
        Map<string, uint32_t>& get();

    private:
        Logger logger;
        vector<Document*> &docs;
        Map<string, uint32_t> bow;
        uint32_t word_id = 1;

        void addToBow(const vector<string>& words);
        void addDocToBow(Document* doc);
        void calcBow();
    };
}

#endif