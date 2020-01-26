#ifndef TERM_FREQUENCY_PROCESSOR_H
#define TERM_FREQUENCY_PROCESSOR_H

#include "src/libs/logger/logger.h"
#include "src/libs/map/map.h"
#include "src/libs/string/string.h"
#include "src/libs/document/Document.h"

namespace BBG {
    class TermFrequencyProcessor {
    public:
        TermFrequencyProcessor(vector<Document*> &docs, const char* filename, Map<string, uint32_t>& bow);
        Map<uint32_t, Map<string, uint32_t>>& get();

    private:
        Logger logger;
        vector<Document*> &docs;
        Map<string, uint32_t>& bow;
        Map<uint32_t, Map<string, uint32_t>> TF;

        void updateTF(Document* doc);
        void calcTF();
    };
}

#endif