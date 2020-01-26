#include "src/libs/frontier/urlQueue/urlQueue.h"

using BBG::string;
using BBG::URLQueue;
using BBG::URL;
using BBG::ShuffleQueue;

// Helper functions

// Handles pushing data to both good and bad quality shuffle queues
void push_to_shuffle(ShuffleQueue &q, const URL& url) {
    if (!q.empty() && q.back().root == url.root)
        q.shuffle_push(url);
    else 
        q.push(url); 
}


// Public member functions

URLQueue::URLQueue(size_t queueSize, size_t badUrlFrequency,
                   size_t seed_ind, size_t inc) :
    goodUrlCount(0), badUrlFrequency(badUrlFrequency),
    goodQuality(queueSize, seed_ind, inc),
    badQuality(queueSize, seed_ind, inc) {}

URLQueue::URLQueue() :
    goodUrlCount(0), badUrlFrequency(0) {}

URLQueue::URLQueue(const URLQueue &other) :
    goodUrlCount(other.goodUrlCount),
    badUrlFrequency(other.badUrlFrequency),
    goodQuality(other.goodQuality),
    badQuality(other.badQuality) {}

URLQueue::~URLQueue() {}

URLQueue& URLQueue::operator=(const URLQueue& other) {
    if (this != &other) {
        goodQuality = other.goodQuality;
        badQuality = other.badQuality;
        goodUrlCount = other.goodUrlCount;
        badUrlFrequency = other.badUrlFrequency;  
    }
    return *this;
}

void URLQueue::setBadFrequency(size_t _badUrlFrequency) {
    if (_badUrlFrequency > 5)
        badUrlFrequency = _badUrlFrequency;
}

size_t URLQueue::size() const {
    return goodQuality.size() + badQuality.size();
}

void URLQueue::push(vector<URL>& urls) {
    for (size_t i = 0; i < urls.size(); ++i)
        add_url(urls[i]);
}

void URLQueue::pop(vector<URL>& urls) {
    for (size_t i = 0; i < urls.size(); ++i)
        get_url(urls[i]);
}

// Private member functions

void URLQueue::pushtoBad(const URL& url) {
    // business as usual
    if (!badQuality.full())
        push_to_shuffle(badQuality, url);
}

void URLQueue::pushtoGood(const URL& url) {
    // If goodQuality is filled with good urls then start putting them
    // in badQuality. If badQuality is full, then pop one off to make room.
    if (goodQuality.full()) {
        if (badQuality.full())
            badQuality.pop();
        pushtoBad(url);
    }
    // Otherwise, business as usual
    else
        push_to_shuffle(goodQuality, url);
}

void URLQueue::add_url(const URL& url) {
    if (url.url.empty() || url.root.empty())
        return;

    if (urlIsGood(url.url))
        pushtoGood(url);
    else
        pushtoBad(url);
 }

// Uses goodUrlCount and badUrlFrequency to pull from either
// ShuffleQueue. Good is chosen more often. Each draw from 
// goodQuality increments goodUrlCount. When goodUrlCount 
// reaches badUrlFrequency, draw from badQuality and reset goodUrlCount.
void URLQueue::get_url(URL& url) {

    ++goodUrlCount %= badUrlFrequency;

    if (goodUrlCount) {
        if (!goodQuality.empty()) {
            BBG::swap(url, goodQuality.front());
            goodQuality.pop();
        }
        else if (!badQuality.empty()) {
            BBG::swap(url, badQuality.front());
            badQuality.pop();
        }
    }
    else {
        if (!badQuality.empty()) {
            BBG::swap(url, badQuality.front());
            badQuality.pop();
        }
        else if (!goodQuality.empty()) {
            BBG::swap(url, goodQuality.front());
            goodQuality.pop();
        }
    }
}
