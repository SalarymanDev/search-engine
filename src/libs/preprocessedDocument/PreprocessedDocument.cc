#include "src/libs/preprocessedDocument/PreprocessedDocument.h"

using BBG::PreprocessedDocument;

PreprocessedDocument::PreprocessedDocument()
    : incomingLinkCount(0), outgoingLinkCount(0), imgCount(0), size(0) {}

void BBG::serialize(const PreprocessedDocument& doc, Buffer& bytes) {
    serialize(doc.url, bytes);
    serialize(doc.titleText, bytes);
    serialize(doc.bodyText, bytes);
    serialize(doc.anchorText, bytes);
    serialize(doc.incomingLinkCount, bytes);
    serialize(doc.outgoingLinkCount, bytes);
    serialize(doc.imgCount, bytes);
    serialize(doc.size, bytes);
    serialize(doc.anchorTermCounts, bytes);
}

void BBG::deserialize(Buffer& bytes, PreprocessedDocument& doc) {
    deserialize(bytes, doc.url);
    deserialize(bytes, doc.titleText);
    deserialize(bytes, doc.bodyText);
    deserialize(bytes, doc.anchorText);
    deserialize(bytes, doc.incomingLinkCount);
    deserialize(bytes, doc.outgoingLinkCount);
    deserialize(bytes, doc.imgCount);
    deserialize(bytes, doc.size);
    deserialize(bytes, doc.anchorTermCounts);
}
