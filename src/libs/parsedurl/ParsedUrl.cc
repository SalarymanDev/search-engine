#include "src/libs/parsedurl/ParsedUrl.h"
#include "src/libs/utils/utils.h"

using BBG::ParsedUrl;
using BBG::strcpy;
using BBG::strlen;

ParsedUrl::ParsedUrl(const string& url) 
    : CompleteUrl(url),
      pathBuffer(new char[ url.size() + 1 ]) {
    strcpy(pathBuffer, url.c_str());
    parse();
}

ParsedUrl::ParsedUrl(const char* url) 
    : CompleteUrl(url),
      pathBuffer(new char[ strlen(url) + 1 ]) {
    strcpy(pathBuffer, url);
    parse();
}

ParsedUrl::~ParsedUrl() {
    delete[] pathBuffer;
}

void ParsedUrl::parse() {
    Service = pathBuffer;

    const char Colon = ':', Slash = '/';
    char *p;
    for ( p = pathBuffer; *p && *p != Colon; p++ ) {}

    if (*p) {
        // Mark the end of the Service.
        *p++ = 0;

        if (*p == Slash) p++;
        if (*p == Slash) p++;

        Host = p;

        for (; *p && *p != Slash && *p != Colon; p++) {}

        if (*p == Colon) {
            // Port specified.  Skip over the colon and
            // the port number.
            *p++ = 0;
            Port = +p;
            for (; *p && *p != Slash; p++) {}
        } 
        else
            Port = p;

        // Mark the end of the Host and Port.
        if (*p) *p++ = 0;

        // Whatever remains is the Path.
        Path = p;
    } 
    else
        Host = Path = p;
}
