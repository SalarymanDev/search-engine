#ifndef INVERSE_DOCUMENT_FREQUENCY_PROCESSOR_H
#define INVERSE_DOCUMENT_FREQUENCY_PROCESSOR_H

#include "src/libs/logger/logger.h"
#include "src/libs/map/map.h"
#include "src/libs/string/string.h"

namespace BBG {
    class InverseDocumentFrequencyProcessor {
    public:
        InverseDocumentFrequencyProcessor(const char *filename, Map<string, uint32_t>& bow, Map<uint32_t, Map<string, uint32_t>>& tf);
        Map<uint32_t, float>& get();

    private:
        Logger logger;
        Map<uint32_t, uint32_t> NK;
        Map<string, uint32_t>& BOW;
        Map<uint32_t, Map<string, uint32_t>>& TF;
        Map<uint32_t, float> IDF;
        float N = 23873;

        void makeNK();
        void updateIDF();
        void calcIDF();
    };
}

#endif