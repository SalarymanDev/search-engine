#include "src/libs/preprocessing/BagOfWordsProcessor.h"
#include "src/libs/preprocessing/TermFrequencyProcessor.h"
#include "src/libs/preprocessing/InverseDocumentFrequencyProcessor.h"
#include "src/libs/preprocessing/NormalizationProcessor.h"
#include "src/libs/preprocessing/AnchorTextProcessor.h"
#include "src/libs/utils/utils.h"
#include "src/libs/logger/logger.h"
#include "src/libs/document/Document.h"

#include <unistd.h>
#include <dirent.h>

using namespace BBG;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
vector<Document*> documents;
Logger log;

void *loadDocuments(void *arg) {
    string *filename = static_cast<string*>(arg);

    FILE *file = fopen(filename->c_str(), "rb");
    if (!file) {
        log.Error("Unable to open file: " + *filename);
        return nullptr;
    }

    log.Info("Reading from file: " + *filename + " ...");

    fseek(file, 0, SEEK_END);
    long filelen = ftell(file);
    rewind(file);

    Buffer buff(filelen);
    fread(buff.modifyData(), 1, filelen, file);

    while (buff.cursor()) {
        Document *temp = new Document();
        try {
            deserialize(buff, *temp);
        } catch (const char*) {
            // log.Error("Encountered error in deserialize. Skipping corrupt file.");
            delete temp;
            break;
        } catch (std::bad_alloc) {
            // log.Error("Encountered bad_alloc in deserialize. Skipping corrupt file.");
            delete temp;
            break;
        } catch (...) {
            log.Error("Encounter unknown error in deserialize. Skipping corrupt file.");
            delete temp;
            break;
        }

        // log.Info(temp->url);
        pthread_mutex_lock(&mutex);
        documents.push_back(temp);
        pthread_mutex_unlock(&mutex);
        temp = nullptr;
    }

    fclose(file);
    log.Info("Reading from file: " + *filename + " complete.");
    return nullptr;
}

void getDocumentFilenames(const string &dirName, vector<string> &filenames, Logger &log) {
    DIR *dir = opendir(dirName.c_str());
    if (!dir)
        log.Fatal("Unable to open directory: " + dirName);

    filenames.clear();

    dirent *entry;
    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_REG && entry->d_type != DT_LNK)
            continue;
        
        filenames.push_back(dirName + "/" + entry->d_name);
    }

    closedir(dir);
}

int main(int argc, char** argv) {
    vector<string> filenames;
    getDocumentFilenames("GoogleDrive/Documents/corpus", filenames, log);

    documents.reserve(10000000);

    pthread_t threads[filenames.size()];
    for (size_t i = 0; i < filenames.size(); ++i)
        pthread_create(&threads[i], nullptr, loadDocuments, &filenames[i]);
    for (size_t i = 0; i < filenames.size(); ++i)
        pthread_join(threads[i], nullptr);

    log.Info("Total Number of Loaded Documents: " + to_string(documents.size()));

    BagOfWordsProcessor bow(documents, "BOW");
    log.Info("Number of unique words: " + to_string(bow.get().size()));

    log.Info("Checking if TF, IDF, and NORM files exist ...");
    if( access( "TF", F_OK ) == -1 || access( "IDF", F_OK ) == -1 || access( "NORM", F_OK ) == -1 ) {
        TermFrequencyProcessor tf(documents, "TF", bow.get());
        InverseDocumentFrequencyProcessor idf("IDF", bow.get(), tf.get());
        NormalizationProcessor norm(documents, "NORM", bow.get(), tf.get(), idf.get());
    } else {
        log.Info("TF, IDF, and NORM files already exist");
    }

    AnchorTextProcessor anchor(documents, bow.get());

    return 0;
}
