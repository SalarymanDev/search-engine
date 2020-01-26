#include "src/libs/frontier/frontier.h"

using BBG::Frontier;
using BBG::URLQueue;
using BBG::lockguard;
using BBG::vector;


// Public member functions

Frontier::Frontier(size_t queueSize, size_t badUrlFrequency,
                   float falsePositiveRate, int expectedElements) 
    : urlqueue(queueSize, badUrlFrequency, 0, 2),
      seenSet(falsePositiveRate, expectedElements) {}


Frontier::~Frontier() {}


size_t Frontier::size() {
    return urlqueue.size();
}


void Frontier::processUrls(vector<URL>& urls, JobType jobtype) {
    switch (jobtype) {
        case (Adding):
            addUrls(urls);
            break;
        case (Removing):
            removeUrls(urls);
            break;
        default:
            break;
    }
}

// Private member functions

void Frontier::addUrls(vector<URL>& urls) {
    for (size_t i = 0; i < urls.size(); ++i) {
        if (robots.violatesRobotsDisallowedPaths(urls[i])) {
            seenSet.markUrlVisited(urls[i].url);
            urls[i].url = "";  
        }
        else if (seenSet.seenUrl(urls[i].url)) {
            logger.Debug("[Frontier]: Url " + urls[i].url + " has been seen before\n");
            urls[i].url = "";
        }
    }
    urlqueue.push(urls);
}

void Frontier::removeUrls(vector<URL>& urls) {
    urlqueue.pop(urls);
    vector<URL> tmp(1);
    for (size_t i = 0; i < urls.size(); ++i) {
        if (robots.violatesRobotsTimeDelay(urls[i])) {
            swap(tmp[0], urls[i]);
            urlqueue.push(tmp);
            urls[i].url = "";
        }
        else {
            robots.updateRobotsAccessedTime(urls[i]);
            seenSet.markUrlProcessing(urls[i].url);
        }
    }
}


void Frontier::markUrlsCrawled(vector<string>& urls) {
    for (size_t i = 0; i < urls.size(); ++i)
        seenSet.markUrlVisited(urls[i]);
}


void Frontier::initFromBackup() {
    char* backupFilename = getenv("BBG_FRONTIER_BACKUP");
    if (!backupFilename)
        logger.Fatal("[Frontier] failed to get env var BBG_FRONTIER_BACKUP filename");

    // Handles deserializtion from backup file
    const string filename(backupFilename);

    FILE* file = fopen(backupFilename, "r");

    // If file doesn't exist. Initialize normally.
    if (file) {
        restoredFromBackup = true;
        logger.Info("[Frontier] restoring from the found backup");
        
        fseek(file, 0, SEEK_END);
        size_t filesize = ftell(file);
        rewind(file);
        
        Buffer bytes(filesize);
        if (fread(bytes.modifyData(), 1, bytes.size(), file) == -1)
            logger.Fatal("[Frontier] unable to open BBG_FRONTIER_BACKUP file: " + filename);
        fclose(file);

        deserialize(bytes, seenSet);
        deserialize(bytes, robots);
    }


    vector<URL> urls;
    urls.reserve(5000000);

    char* seedlist = getenv("BBG_SEED_LIST");
    if (!seedlist)
        logger.Fatal("[Frontier] failed to get env var BBG_SEED_LIST filename");

    file = fopen(seedlist, "r");
    if (!file)
      logger.Fatal("No seedlist");

    // Read seedlist into memory
    char c;
    string url;
    while((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            urls.push_back({url, getRoot(url)});
            url.clear();
            continue;
        }

        url.push_back(c);
    }
    fclose(file);

    // Add seedlist/backup to frontier
    if (restoredFromBackup)
        urlqueue.push(urls);
    else
        processUrls(urls, JobType::Adding);
}

void Frontier::backup() {

    logger.Info("[Frontier::backup] backing up frontier");

    vector<URL> urls(size());
    urlqueue.pop(urls);

    char* seedlist = getenv("BBG_SEED_LIST");
    if (!seedlist)
        logger.Fatal("[Frontier] failed to get env var BBG_SEED_LIST filename");

    string seedlistTmp(seedlist);
    seedlistTmp += ".tmp";

    FILE *file = fopen(seedlistTmp.c_str(), "w+");
    
    if (!file) {
        logger.Error("Failed to backup frontier");
        return;
    }

    // Write frontier urls to tmp file
    for (size_t i = 0; i < urls.size(); ++i) {
        fwrite(urls[i].url.c_str(), 1, urls[i].url.size(), file);
        fwrite("\n", 1, 1, file);
    }

    fclose(file);

    rename(seedlistTmp.c_str(), seedlist);

    char* backupFilename = getenv("BBG_FRONTIER_BACKUP");
    if (!backupFilename)
        logger.Fatal("[Frontier] failed to get env var BBG_FRONTIER_BACKUP filename");

    file = fopen(backupFilename, "w+");
    if (!file)
        logger.Fatal("[Frontier] failed to open file BBG_FRONTIER_BACKUP env var");

        
    Buffer bytes;
    serialize(seenSet, bytes);
    serialize(robots, bytes);
    
    if (fwrite(bytes.data(), 1, bytes.size(), file) == -1)
        logger.Fatal("[Frontier] unable to write to BBG_FRONTIER_BACKUP file: " + string(backupFilename));

    fclose(file);

    urlqueue.push(urls);
    
    logger.Info("[Frontier::backup] frontier backup complete");
}

