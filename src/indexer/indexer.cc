#include "src/libs/index/Indexer/Indexer.h"
#include "src/libs/vector/vector.h"
#include "src/libs/string/string.h"
#include "src/libs/preprocessedDocument/PreprocessedDocument.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/serialize/Buffer.h"
#include "src/libs/logger/logger.h"
#include "src/libs/preprocessing/loaders/loaders.h"

#include <dirent.h>
#include <unistd.h>

#define MAX_NUM_THREADS 1
#define NUM_FILES_PER_CHUNK 10

using namespace BBG;

Logger logg;

Map<string, uint32_t> bow;
Map<uint32_t, Map<string, uint32_t>> tf;
Map<uint32_t, float> idf;
Map<string, float> norm;

struct Args {
    vector<string> *filenames;
    string *chunkFilename;
};

struct Thread {
    pthread_t *thread;
    Args *args;

    Thread() : thread(nullptr), args(nullptr) {}
    Thread(pthread_t *thread, Args *args) : thread(thread), args(args) {}
};

void getDocumentFilenames(const string &dirName, vector<string> &filenames) {
    DIR *dir = opendir(dirName.c_str());
    if (!dir)
        logg.Fatal("Unable to open directory: " + dirName);

    dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_REG && entry->d_type != DT_LNK)
            continue;
        
        filenames.push_back(dirName + "/" + entry->d_name);
    }

    closedir(dir);
}

void loadDocuments(const string &filename, vector<PreprocessedDocument> &documents) {
    FILE *file = fopen(filename.c_str(), "rb");
    if (!file)
        logg.Fatal("Cannot open file: " + filename);

    fseek(file, 0, SEEK_END);
    long fileLength = ftell(file);
    rewind(file);

    Buffer bytes(fileLength);
    fread(bytes.modifyData(), 1, bytes.size(), file);
    fclose(file);

    PreprocessedDocument current;
    while (bytes.cursor()) {
        deserialize(bytes, current);
        documents.push_back(current);
    }
}

void *indexDocuments(void *args) {
    Args *arg = static_cast<Args*>(args);
    vector<string> *filenames = arg->filenames;
    string *chunkFilename = arg->chunkFilename;

    vector<PreprocessedDocument> documents;
    logg.Info("Loading chunk data " + *chunkFilename + " ...");
    for (size_t i = 0; i < filenames->size(); ++i)
        loadDocuments((*filenames)[i], documents);
    logg.Info("Loading chunk data " + *chunkFilename + " complete");

    if (!documents.empty()) {
        logg.Info("Creating chunk: " + *chunkFilename + " ...");
        Indexer indexer(bow, tf, idf, norm);
        indexer.createIndexChunk(chunkFilename->c_str(), documents);
        logg.Info("Creating chunk: " + *chunkFilename + " complete");
    }

    delete chunkFilename;
    delete filenames;
    delete arg;
    return nullptr;
}


int main(int argc, char** argv) {
    logg.Info("Loading bow ...");
    loadBow("BOW", bow);
    logg.Info("Loading bow complete");
    logg.Info("Loading tf ...");
    loadTF("TF", tf);
    logg.Info("Loading tf complete");
    logg.Info("Loading idf ...");
    loadIDF("IDF", idf);
    logg.Info("Loading idf complete");
    logg.Info("Loading norm ...");
    loadNorm("NORM", norm);
    logg.Info("Loading norm complete");
    
    vector<string> filenames;
    getDocumentFilenames("GoogleDrive/preindex", filenames);
    for (size_t i = 0; i < filenames.size(); ++i)
        logg.Info("Found: " + filenames[i]);

    // Create thread pool
    const string filename = "GoogleDrive/index/index-chunk-";
    unsigned int chunkNum = 0;
    vector<Thread> threadPool;
    threadPool.reserve(MAX_NUM_THREADS);

    while (!filenames.empty()) {
        for (int i = 0; i < MAX_NUM_THREADS; ++i) {
            if (filenames.empty()) break;

            Args *args = new Args();
            args->chunkFilename = new string(filename + to_string(chunkNum++));
            args->filenames = new vector<string>();

            for (int j = 0; j < NUM_FILES_PER_CHUNK; ++j) {
                if (filenames.empty()) break;
                args->filenames->push_back(filenames.back());
                filenames.pop_back();
            }

            pthread_t *thread = new pthread_t();
            threadPool.push_back(Thread(thread, args));
        }

        logg.Info("Creating " + to_string(threadPool.size()) + " new threads");
        for (int i = 0; i < threadPool.size(); ++i)
            pthread_create(threadPool[i].thread, nullptr, indexDocuments, threadPool[i].args);

        for (int i = 0; i < threadPool.size(); ++i) {
            pthread_join(*(threadPool[i].thread), nullptr);
            delete threadPool[i].thread;
        }

        threadPool.clear();
    }

    return 0;
}
