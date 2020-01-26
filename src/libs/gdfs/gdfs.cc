#include "src/libs/gdfs/gdfs.h"
#include "src/libs/logger/logger.h"
#include "src/libs/serialize/Buffer.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/thread/lockguard.h"

#include <cstring>

#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>

using namespace BBG;

GDFS::GDFS()
    : mutex(PTHREAD_MUTEX_INITIALIZER), fp(nullptr), numDocs(0), counter(0) {
  hostname[HOST_NAME_MAX] = '\0';
  gethostname(hostname, HOST_NAME_MAX);
}

GDFS::~GDFS() {
  if (fp) fclose(fp);
}

// date should be a string in the form "YYYY-MM-DD"
void GDFS::getFilePaths(const string &date, vector<string> &filenames) {
  string path = "GoogleDrive/Documents/";
  path += date;
  path += '/';

  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(path.c_str())) == NULL)
    log.Error("[GDFS::getFilePaths] Couldn't open directory" + path);

  while ((ent = readdir(dir)) != NULL) {
    if (ent->d_name[0] == '.') continue;
    filenames.push_back(path + ent->d_name);
  }
}

void GDFS::getDocuments(const string &filename, vector<Document> &docs) {
  if (fp) fclose(fp);

  FILE *fp = fopen(filename.c_str(), "rb");
  if (!fp)
    log.Error("[GDFS::getDocuments] Couldn't open file " + filename);
  else
    log.Info("[GDFS::getDocuments] Reading file " + filename);

  log.Info("Getting file length");
  fseek(fp, 0, SEEK_END);
  long filelen = ftell(fp);
  rewind(fp);

  log.Info("Reading into buffer");
  Buffer buff(filelen);
  fread(buff.modifyData(), 1, filelen, fp);

  log.Info("Deserialising into vector");
  docs.clear();
  docs.reserve(MAX_DOCS_PER_FILE);
  Document temp;
  while (buff.cursor()) {
    try {
      deserialize(buff, temp);
    } catch (const char* error) {
      log.Error("Failed to deserialize a document");
      return;
    } catch (std::bad_alloc& ba) {
      log.Error("Failed to deserialize a document due to bad_alloc");
      return;
    }
    docs.push_back(temp);
  }
}

void GDFS::addDocument(const Document &doc) {
  lockguard lock(&mutex);  // RAII

  // New file if this is the first time addDoc is being called or if
  // the file is at capacity
  if (!fp || numDocs >= MAX_DOCS_PER_FILE) newFile();

  // Even if serialisation fails part way through we still
  // want to increment this so we don't stick to one file
  ++numDocs;

  // serialise and append document to the file
  Buffer buff;
  serialize(doc, buff);
  fwrite(buff.data(), 1, buff.size(), fp);

  char c[9];
  if (!((numDocs + (counter * MAX_DOCS_PER_FILE)) % 1000)) {
    logline = "[GDFS] added url document number: ";
    sprintf(c, "%zu", (numDocs + (counter * MAX_DOCS_PER_FILE)));
    logline += c;
    log.Info(logline);
  }
}

// Make a new file GoogleDrive/Documents/DATE/HOSTNAME_COUNTER, return file
// pointer
void GDFS::newFile() {
  if (fp) fclose(fp);

  numDocs = 0;

  time(&rt);
  strftime(ts, 20, "%F", gmtime(&rt));
  char exactTime[20];
  strftime(exactTime, 20, "%T", gmtime(&rt));

  int buffsize = sizeof(unsigned long) * 8 + 1;
  char buffer[buffsize];
  snprintf(buffer, buffsize, "%lu", counter);
  
  filename = "GoogleDrive/Documents/";
  filename += ts;
  filename += "/";
  filename += hostname;
  filename += "_";
  filename += exactTime;
  filename += "_";
  filename += buffer;
  fp = fopen(filename.c_str(), "a");

  if (!fp) {
    printf("%s\n", strerror(errno));
    log.Fatal("[GDFS::newFile] Couldn't open file " + filename);
  } else
    log.Info("[GDFS::newFile] Created file " + filename);

  ++counter;
}
