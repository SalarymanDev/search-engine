#ifndef GDFS_H
#define GDFS_H

#include "src/libs/logger/logger.h"
#include "src/libs/string/string.h"
#include "src/libs/document/Document.h"

#include <pthread.h>
#include <limits.h>
#include <stdio.h>
#include <time.h>

namespace BBG {
  class GDFS {
  private:
    constexpr static int MAX_DOCS_PER_FILE = 5000;
    Logger log;

    pthread_mutex_t mutex;

    time_t rt;
    char ts[20];
    char hostname[HOST_NAME_MAX + 1];
    string filename;
    FILE* fp;
    int numDocs;
    unsigned long counter;
    string logline;

    void newFile();

  public:
    GDFS();
    ~GDFS();
    void addDocument(const Document& doc);
    void getDocuments(const string &filename, vector<Document> &docs);
    void getFilePaths(const string &date, vector<string> &filenames);

    class Iterator {
    private:
      friend class GDFS;
      GDFS* gdfs;
      vector<string> files;
      vector<Document> docs;
      size_t currentDoc;
      size_t currentFile;

    public:
      Iterator(string& date, GDFS* fs) : gdfs(fs), currentDoc(0), currentFile(0) {
        gdfs->getFilePaths(date, files);
        gdfs->getDocuments(files[currentFile], docs);
      }

      Iterator(const Iterator &other, GDFS* fs) : gdfs(fs), files(other.files),
          docs(other.docs), currentDoc(other.currentDoc), currentFile(other.currentFile) { }

      // End iterator
      Iterator() : gdfs(nullptr), currentDoc(0), currentFile(0) { }

      Document& operator*() {
        return docs[currentDoc];
      }

      Document* operator->() {
        return &docs[currentDoc];
      }

      Iterator &operator++() {
        if (currentDoc == docs.size() - 1) {
          if (currentFile == files.size() - 1) {
            gdfs = nullptr;
            currentFile = 0;
            currentDoc = 0;
            files.clear();
          } else {
            // TODO: might break if the file doesn't have 5000 elements
            gdfs->getDocuments(files[++currentFile], docs);
            currentDoc = 0;
          }
        } else {
          ++currentDoc;
        }
        return *this;
      }

      Iterator operator++(int) {
        Iterator i(*this);
        ++(*this);
        return i;
      }

      bool operator==(const Iterator &rhs) const {
        return gdfs == rhs.gdfs && currentFile == rhs.currentFile &&
          currentDoc == rhs.currentDoc && files == rhs.files;
      }

      bool operator!=(const Iterator &rhs) const {
        return gdfs != rhs.gdfs || currentFile != rhs.currentFile ||
          currentDoc != rhs.currentDoc || files != rhs.files;
      }
    };

    Iterator begin(string& date) {
      return Iterator(date, this);
    }

    Iterator end() {
      return Iterator();
    }
  };
}

#endif
