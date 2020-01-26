#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "src/libs/map/map.h"
#include "src/libs/string/string.h"
#include "src/libs/serialize/Buffer.h"

namespace BBG {
    struct HttpResponse {
        enum StatusCode {
            OK = 200,
            NOT_FOUND = 404,
            BAD_REQUEST = 400
        };

        StatusCode status;
        Map<string, string> headers;
        string body;
        bool isValid;

        HttpResponse();
        HttpResponse(string response);
        string toString();
    };
}

#endif