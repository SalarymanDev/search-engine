#include "src/libs/httpServer/HttpResponse.h"
#include <iostream>

using BBG::HttpResponse;
using BBG::string;

HttpResponse::HttpResponse() {}

HttpResponse::HttpResponse(string response) {
    isValid = false;
    if (response.empty()) return;
    const char* temp = response.c_str();

    while(*temp && *temp != ' ') ++temp;
    if (!*temp) return;
    ++temp;

    std::string statusString;
    while(*temp && *temp != ' ') statusString += *temp++;
    int statusNum = atoi(statusString.c_str());
    status = static_cast<StatusCode>(statusNum);

    while(*temp && *temp != '\n') ++temp;
    if (!*temp) return;
    ++temp;

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
    body.append(temp, size);

    isValid = true;
}

string HttpResponse::toString() {
    string response("HTTP/1.0 ");
    switch (status) {
        case StatusCode::OK:
            response += "200 OK";
            break;
        case StatusCode::NOT_FOUND:
            response += "404 NOT FOUND";
            break;
        default:
            response += "400 BAD REQUEST";
            break;
    }

    if (!body.empty()) {
        char bodySizeStr[20];
        sprintf(bodySizeStr, "%d", (int)body.size());
        headers["Content-Length"] = string(bodySizeStr);
    }

    for (auto iter = headers.begin(); iter; iter = headers.next()) {
        response += "\r\n";
        response += iter->key;
        response += ": ";
        response += iter->val;
    }

    response += "\r\n\r\n";

    if (!body.empty())
        response += body;

    return response;
}