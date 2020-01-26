#include "src/libs/gdfs/gdfs.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/document/Document.h"
#include "src/libs/serialize/Buffer.h"
#include "src/libs/serialize/serialize.h"
#include "tests/catch.hpp"

#include <experimental/filesystem>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>

using namespace BBG;

time_t rt;
char ts[20];

void initFS() {
  time(&rt);
  strftime(ts, 20, "%F", gmtime(&rt));
  string path("GoogleDrive/Documents/");
  path += ts;

  std::experimental::filesystem::remove_all("GoogleDrive");
  std::experimental::filesystem::create_directories(path.c_str());
}

void addFileToBuffer(const char* filename, Buffer &buff) {
  FILE *fp = fopen(filename, "rb");
  fseek(fp, 0, SEEK_END);
  long filelen = ftell(fp);
  rewind(fp);

  char * filedata = new char[filelen];
  fread(filedata, 1, filelen, fp);
  buff.append(filedata, filelen);
  delete[] filedata;

  printf("Read file to buffer: %s\n", filename);
}

TEST_CASE("Single file read/write test", "[gdfs]") {
  printf("Single file test starting...\n");
  initFS();

  string title("document title");
	string body("document\nbody\nmore\ttext to test");
	string anchorUrl("anchor url ");
	string anchorText("anchor text ");

	Document doc("yEET");
  doc.titleText.push_back(title);
  doc.bodyText.push_back(body);
  doc.anchors[anchorUrl].push_back(anchorText);
  doc.anchors[anchorUrl].push_back(anchorText);
  doc.anchors[anchorUrl].push_back(anchorText);

  Buffer expected;
  serialize(doc, expected);
  int numFiles = 1;

  GDFS fs;
  for (int i = 0; i < numFiles * 5000; ++i)
    fs.addDocument(doc);

  vector<string> filenames;
  vector<Document> docs;
  string time(ts);

  fs.getFilePaths(time, filenames);
  fs.getDocuments(filenames[0], docs);

  for (int i = 0; i < 5000; ++i)
    REQUIRE(docs[i] == doc);
}

TEST_CASE("Multi file test", "[gdfs]") {
  printf("Multi file test starting...\n");
  initFS();

  string title("document title");
	string body("document\nbody\nmore\ttext to test");
	string anchorUrl("anchor url ");
	string anchorText("anchor text ");

	Document doc("yEET");
  doc.titleText.push_back(title);
  doc.bodyText.push_back(body);

  Buffer expected;
  int numFiles = 6;

  // In its own scope to call destructor and close the file
  // so we can open it here nicely
  {
    GDFS fs;
    for (int i = 0; i < numFiles * 5000; ++i) {
      if (i % 64 == 0)
        doc.anchors[anchorUrl].push_back(anchorText);
      fs.addDocument(doc);
      serialize(doc, expected);
    }
  }

  GDFS iterFS;
  string date(ts);
  Buffer actual;
  vector<string> filenames;
  iterFS.getFilePaths(date, filenames);
  for (int i = 0; i < numFiles; ++i)
    addFileToBuffer(filenames[i].c_str(), actual);

  // file ordering is different
  /* REQUIRE(actual == expected); */

  Document actualDoc;
  int c = 0;
  for (auto i = iterFS.begin(date); i != iterFS.end(); ++i) {
    deserialize(actual, actualDoc);
    REQUIRE(*i == actualDoc);
    c++;
  }
}

static GDFS staticFS;

// Each thread will add 5000 simple documents (1 files worth)
void* threadFuncSimple(void* arg) {
  int num = *((int*) arg);
  printf("Thread %d starting...\n", num);

  Document doc;
  string title("title");
	string body("doc\nbody\nmore\ttext to test");
	string anchorUrl("anchor url ");
	string anchorText("anchor text ");
	for (int i = 0; i < num; ++i)
    doc.anchors[anchorUrl].push_back(anchorText);

  for (int i = 0; i < 5000; ++i)
    staticFS.addDocument(doc);

  printf("Thread %d done\n", num);
  return nullptr;
}

TEST_CASE("Multi threaded multi file test", "[gdfs]") {
  printf("Multi threaded test starting...\n");
  initFS();

  int numThreads = 5;
  pthread_t threads[numThreads];
  int ints[numThreads];

  // Launch and join on pthreads
  for (int i = 0; i < numThreads; ++i) {
    ints[i] = i;
    pthread_create(&threads[i], NULL, threadFuncSimple, (void*) &ints[i]);
  }

  for (int i = 0; i < numThreads; ++i)
    pthread_join(threads[i], NULL);

  printf("All threads finished\n");

  // Close last file
  staticFS.~GDFS();

  printf("GDFS destructor called\n");

  GDFS fs;
  Buffer actual;
  vector<Document> docs;
  vector<Document> actualDocs;
  vector<string> filenames;
  fs.getFilePaths(ts, filenames);

  // Go over each file and make sure we're reading them in correctly
  for (int i = 0; i < numThreads; ++i) {
    docs.resize(5000);
    actualDocs.resize(5000);
    fs.getDocuments(filenames[i], docs);
    addFileToBuffer(filenames[i].c_str(), actual);
    for (int i = 0; i < 5000; ++i) {
      deserialize(actual, actualDocs[i]);
    }
    REQUIRE(docs == actualDocs);
    docs.clear();
    actualDocs.clear();
    actual.clear();
  }

  REQUIRE(true);
}
