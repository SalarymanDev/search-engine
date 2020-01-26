#ifndef PREPROCESSED_DOCUMENT_H
#define PREPROCESSED_DOCUMENT_H

#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/map/map.h"

namespace BBG {
    class PreprocessedDocument {
    public:
        string url;
        vector<string> titleText;
        vector<string> bodyText;
        vector<string> anchorText;
        size_t incomingLinkCount;
        size_t outgoingLinkCount;
        size_t imgCount;
        size_t size;
        Map<uint32_t, uint32_t> anchorTermCounts;

        PreprocessedDocument();

        friend void serialize(const PreprocessedDocument& doc, Buffer& bytes);
        friend void deserialize(Buffer& bytes, PreprocessedDocument& doc);
    };

    void serialize(const PreprocessedDocument& doc, Buffer& bytes);
    void deserialize(Buffer& bytes, PreprocessedDocument& doc);
}

#endif