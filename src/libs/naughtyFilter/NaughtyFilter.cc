#include "src/libs/naughtyFilter/NaughtyFilter.h"
#include <string>
#include <iostream>
#include <unistd.h>

using namespace BBG;

NaughtyFilter::NaughtyFilter(bool useStopwordList) {
    const char *stopwordlist = "/home/braceal/webserver/stopwords.txt";//getenv("BBG_STOPWORDS");
    const char *blacklist = "/home/braceal/webserver/blacklist.txt";//getenv("BBG_BLACKLIST");

    if (!stopwordlist) logger.Fatal("[NaughtyFilter:Constructor] Env Variable BBG_STOPWORDS is missing!");
    if (!blacklist) logger.Fatal("[NaughtyFilter:Constructor] Env Variable BBG_BLACKLIST is missing!");

    string listname = useStopwordList ? stopwordlist : blacklist;

    FILE *file = fopen(listname.c_str(), "r");
    if (!file) logger.Fatal("[NaughtyFilter:Constructor] Unable to open word list: " + listname);

    string line;
    char current;
    while((current = fgetc(file)) != EOF) {
        if (current == '\n') {
            if (line.empty()) continue;
            naughtySet.insert(line, true);
            line.clear();
            continue;
        }

        line += current;
    }

    fclose(file);
}

bool NaughtyFilter::isNaughty(const string& word) const {
    return naughtySet.contains(word);
}
