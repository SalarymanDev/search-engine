#ifndef URL_QUEUE_H
#define URL_QUEUE_H

#include <pthread.h>
#include <cstddef> // size_t
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/parsedurl/ParsedUrl.h"
#include "src/libs/frontier/rank/rank.h"
#include "src/libs/frontier/shuffleQueue/shuffleQueue.h"
#include "src/libs/frontier/URL/URL.h"

namespace BBG {

    class URLQueue {
    private:

        // The following are used to choose URLs from either queue. 
        // Good quality is chosen more often. Each draw from goodQuality 
        // queue increments goodUrlCount. When goodUrlCount reaches 
        // badUrlFrequency, draws from low value queue and reset goodUrlCount
        size_t goodUrlCount;
        size_t badUrlFrequency;

        ShuffleQueue goodQuality;
        ShuffleQueue badQuality;

        // Checks quality to choose which ShuffleQueue to add too
        void add_url(const URL& url);

        // Checks quality with goodUrlCount and badUrlFrequency
        // Will increment goodUrlCount and mod badUrlFrequency
        void get_url(URL& url);

        void pushtoGood(const URL& url);
        void pushtoBad(const URL& url);

    public:

        URLQueue();
        ~URLQueue();
        // Initializes ShuffleQueue's with starting index for
        // shuffles and the increment to adjust the index after
        // each shuffle. Starts running push_batch, pop_batch threads.
        URLQueue(size_t queueSize, size_t badUrlFrequency,
                 size_t seed_ind, size_t inc);

        URLQueue(const URLQueue& other);
        URLQueue& operator=(const URLQueue& other); 

        void setBadFrequency(size_t _badUrlFrequency);
        // Returns sum of each of the stacks
        size_t size() const;  
        
        // Puts/gets URLs in args
        void push(vector<URL>& urls);
        void pop(vector<URL>& urls); 

    };
}

#endif
