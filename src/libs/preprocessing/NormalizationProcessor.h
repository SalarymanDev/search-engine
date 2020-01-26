#ifndef NORMALIZATION_PROCESSOR_H
#define NORMALIZATION_PROCESSOR_H

#include "src/libs/logger/logger.h"
#include "src/libs/string/string.h"
#include "src/libs/map/map.h"
#include "src/libs/document/Document.h"

namespace BBG {
    class NormalizationProcessor {
    public:
        NormalizationProcessor(vector<Document*> &docs, const char *filename, Map<string, uint32_t>& bow, Map<uint32_t, Map<string, uint32_t>>& tf, Map<uint32_t, float>& idf);
        Map<string, float>& get();

    private:
        Logger logger;
        vector<Document*> &docs;
        Map<string, uint32_t>& BOW;
        Map<uint32_t, Map<string, uint32_t>>& TF;
        Map<uint32_t, float>& IDF;
        Map<string, float> NORM;

        void updateNorm(Document& doc);
        void calcNorm();
    };
}

#endif