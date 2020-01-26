#ifndef ANCHOR_TEXT_PROCESSOR_H
#define ANCHOR_TEXT_PROCESSOR_H

#include "src/libs/logger/logger.h"
#include "src/libs/map/map.h"
#include "src/libs/preprocessedDocument/PreprocessedDocument.h"
#include "src/libs/document/Document.h"

namespace BBG {
    class AnchorTextProcessor {
    public:
    AnchorTextProcessor(vector<Document*> &docs, Map<string, uint32_t>& _bow);

    private:
        Logger logger;
        vector<Document*> &docs;
        Map<string, uint32_t>& bow;
        Map<string, PreprocessedDocument> processedDocuments;

        void run();
        void write();
    };
}

#endif