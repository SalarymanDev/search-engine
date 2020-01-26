#ifndef NAUGHTY_FILTER_H
#define NAUGHTY_FILTER_H

#include "src/libs/string/string.h"
#include "src/libs/map/map.h"
#include "src/libs/logger/logger.h"

namespace BBG {
    class NaughtyFilter {
    public:
        NaughtyFilter(bool useStopwordList);
        bool isNaughty(const string& word) const;

    private:
        Map<string, bool> naughtySet;
        Logger logger;
    };
}

#endif