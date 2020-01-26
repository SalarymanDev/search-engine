#include "src/libs/httpServer/HttpRequest.h"
#include <iostream>

using BBG::HttpRequest;
using BBG::string;

string HttpRequest::endpointKey() {
    string key(path);
    key += ' ';
    key += method;
    return key;
}

HttpRequest::HttpRequest(const string& request) {
    isValid = false;
    if (request.empty()) return;
    const char* temp = request.c_str();

    while (*temp && *temp != ' ') method += *temp++;
    if (!*temp) return;
    temp++;

    while (*temp && *temp != ' ') path += *temp++;
    if (!*temp) return;
    temp++;

    while (*temp && *temp != '\r') httpVersion += *temp++;
    if (!*temp || !*(temp + 1)) return;
    temp += 2;

    string header, value;
    while (*temp && *temp != '\r') {
        while (*temp && *temp != ':') header += *temp++;
        if (!*temp || !*(temp + 1)) return;
        temp += 2;

        while (*temp && *temp != '\r') value += *temp++;

        headers[header] = value;
        header.clear();
        value.clear();

        if (!*temp || !*(temp + 1)) return;
        temp += 2;
    }

    if (!headers.contains("Content-Length")) {
        isValid = true;
        return;
    }

    size_t size = strtoul(headers["Content-Length"].c_str(), NULL, 10);
    if (!*temp || !*(temp + 1)) return;
    temp += 2;
    for (size_t i = 0; i < size; ++i) {
        if (!temp[i]) return;
        body += temp[i];
    }

    isValid = true;
}
