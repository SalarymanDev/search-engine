#include "src/libs/preprocessing/AnchorTextProcessor.h"
#include "src/libs/gdfs/gdfs.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"

using BBG::AnchorTextProcessor;
using BBG::GDFS;

AnchorTextProcessor::AnchorTextProcessor(vector<Document*> &docs, Map<string, uint32_t>& _bow)
    : docs(docs), bow(_bow) {
        logger.Info("Processing anchor text...");
        run();
        logger.Info("Processing anchor text complete");
        logger.Info("Processed a total number of " + to_string(processedDocuments.size()) + " preprocessed documents.");
        logger.Info("Writing anchor text to disk...");
        write();
        logger.Info("Writing anchor text to disk complete");
    }

void AnchorTextProcessor::run() {    
    for (size_t i = 0; i < docs.size(); ++i) {
        PreprocessedDocument &currentDocument = processedDocuments[docs[i]->url];

        currentDocument.titleText = docs[i]->titleText;
        currentDocument.bodyText = docs[i]->bodyText;
        currentDocument.url = docs[i]->url;
        currentDocument.size = docs[i]->size;
        currentDocument.imgCount = docs[i]->imgCount;
        currentDocument.outgoingLinkCount = docs[i]->anchors.size();

        for (auto *entry = docs[i]->anchors.begin(); entry; entry = docs[i]->anchors.next()) {
            PreprocessedDocument &linkedDocument = processedDocuments[entry->key];
            
            if (linkedDocument.url.empty())
                linkedDocument.url = entry->key;

            for (int i = 0; i < entry->val.size(); ++i) { 
                linkedDocument.anchorText.push_back(entry->val[i]);
                
                // anchor counts for linkedDocument
                ++linkedDocument.anchorTermCounts[bow[entry->val[i]]];
            }
            linkedDocument.incomingLinkCount++;
        }
        
        delete docs[i];
    }
}

void AnchorTextProcessor::write() {
    int filenum = 0;
    string filename("GoogleDrive/preindex/processed-documents-" + to_string(filenum++));
    FILE *file = fopen(filename.c_str(), "w");
    
    if (!file)
        logger.Fatal("Cannot open file: " + filename);

    // Save processed documents
    size_t totalPreprocessedDocuments = processedDocuments.size();
    int i = 0;
    Buffer bytes;
    for (auto *document = processedDocuments.begin(); document; document = processedDocuments.next()) {
        serialize(document->val, bytes);
        ++i;

        if (i == 10000) {
            fwrite(bytes.data(), 1, bytes.size(), file);
            fclose(file);

            file = nullptr;
            bytes.clear();
            i = 0;

            filename = "GoogleDrive/preindex/processed-documents-" + to_string(filenum++);
            file = fopen(filename.c_str(), "a+");
            if (!file)
                logger.Fatal("Cannot open file: " + filename);
        }
    }

    if (!bytes.empty()) {
        fwrite(bytes.data(), 1, bytes.size(), file);
        fclose(file);
    }
}