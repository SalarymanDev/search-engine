#ifndef SHUFFLE_QUEUE_H
#define SHUFFLE_QUEUE_H

#include <cstddef> // size_t
#include <cassert>
#include "src/libs/string/string.h"
#include "src/libs/queue/queue.h"
#include "src/libs/utils/utils.h"
#include "src/libs/frontier/URL/URL.h"

namespace BBG {

    class ShuffleQueue : public queue<URL> {
    private:
        // ind stores next indices for shuffle_push
        // inc stores amount to increment ind by after shuffle_push
        size_t ind, inc;

    public:
        ShuffleQueue();
        ShuffleQueue(size_t size, size_t ind, size_t inc);

        ShuffleQueue(const ShuffleQueue& other);
        ShuffleQueue& operator=(const ShuffleQueue& other); 

        void shuffle_push(const URL& val);
    };

} // BBG

#endif
