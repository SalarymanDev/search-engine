#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include "src/libs/map/map.h"
#include "src/libs/string/string.h"

namespace BBG {
    struct HttpRequest {
        string method;
        string path;
        string httpVersion;
        Map<string, string> headers;
        string body;
        bool isValid;

        string endpointKey();
        HttpRequest(const string& request);
    };
}

#endif