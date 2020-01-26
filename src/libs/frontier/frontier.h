#ifndef FRONTIER_H
#define FRONTIER_H

#include <cstddef> // size_t
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/logger/logger.h"
#include "src/libs/frontier/urlQueue/urlQueue.h"
#include "src/libs/frontier/seen/seen.h"
#include "src/libs/robotsManager/robotsManager.h"
#include "src/libs/frontier/URL/URL.h"

namespace BBG {

    enum JobType {
      Adding,
      Removing
    };

    class Frontier{
    private:
        URLQueue urlqueue;

        RobotsManager robots;
        SeenSet seenSet;

        Logger logger;
        

        // Job handlers
        void addUrls(vector<URL>& urls);
        void removeUrls(vector<URL>& urls);
        void backUpUrls(vector<URL>& urls);

    public:

        // Constructs numQueues URLQueue objects each of size queueSize        
        Frontier(size_t queueSize, size_t badUrlFrequency,
                 float falsePositiveRate, int expectedElements);

        ~Frontier();

        // Size
        size_t size();

        // Safely shutdown frontier
        void shutdown();

        // Backs up frontier data structures seenSet and robots on start up
        // if backup file exists.
        void backup();
        void initFromBackup();

        // Adds or gets urls from the frontier. 
        void processUrls(vector<URL>& urls, JobType jobtype);
        void markUrlsCrawled(vector<string>& urls);
        
        bool restoredFromBackup = false;
    };
}

#endif
