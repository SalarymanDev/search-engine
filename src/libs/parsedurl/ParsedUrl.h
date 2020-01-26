#ifndef PARSED_URL_H
#define PARSED_URL_H

#include "src/libs/string/string.h"

namespace BBG {
    class ParsedUrl {
    public:
        const string CompleteUrl;
        char *Service, *Host, *Port, *Path;

        ParsedUrl(const string& url);
        ParsedUrl(const char* url);
        ~ParsedUrl();

    private:
        char *pathBuffer;

        void parse();
    };
}

#endif
