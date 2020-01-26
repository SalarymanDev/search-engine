#include "src/libs/rpc/RpcServer.h"
#include "src/libs/serialize/serialize.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netdb.h"
#include <unistd.h>
#include <iostream>

using BBG::RpcServer;
using BBG::serialize;
using BBG::Buffer;
using BBG::Logger;
using BBG::vector;
using BBG::string;
using BBG::Map;
using std::cout;
using std::endl;

RpcServer::RpcServer(int port, int listenQueueSize, int retry) : 
    Server(port, (void*(*)(void*))&RpcServer::requestHandler, listenQueueSize, retry) {}

void RpcServer::registerEndpoint(string keyword, Buffer (*handler)(Buffer&)) {
    if (_listener != 0)
        _logger.Fatal("Rpc Error: Cannot register endpoints after starting server.");

    _endpoints[keyword] = handler;
}

void* RpcServer::requestHandler(void* arg) {
    args_struct *args = static_cast<args_struct*>(arg);
    int *msgsock = args->msgsock;
    RpcServer *self = static_cast<RpcServer*>(args->self);
    delete args;

    string keyword;
    char blocking;
    size_t bytes, msgsize;
    while ((bytes = recv(*msgsock, &msgsize, sizeof(size_t), MSG_WAITALL)) > 0) {
        keyword.clear();

        char buffer[msgsize];
        bytes = recv(*msgsock, buffer, msgsize, MSG_WAITALL);
        size_t i = 0;
        // Read keyword
        while (buffer[i] && buffer[i] != ' ') keyword.push_back(buffer[i++]);

        // Skip space, read blocking char, skip space
        ++i;
        blocking = buffer[i++];
        ++i;

        // Read request
        Buffer request;
        while (i < bytes) request.push_back(buffer[i++]);

        Buffer response;
        if (self->_endpoints.contains(keyword)) {
            // Run endpoint code and get response
            Buffer (*endpoint)(Buffer&) = *(self->_endpoints[keyword]);
            response = endpoint(request);
        } else {
            // Endpoint no found error response
            string keywordError("Error: Endpoint for '");
            keywordError += keyword.c_str();
            keywordError += "' not found.";
            self->_logger.Error(keywordError);
            serialize(keywordError, response);
        }

        if (blocking == 'n')
            continue;

        int attempts = 0;
        size_t responseSize = response.size();
        ssize_t sendResult;
        do {
            sendResult = send(*msgsock, &responseSize, sizeof(size_t), 0);
        } while (sendResult == -1 && ++attempts < self->_retry);

        if (sendResult == -1) {
            perror("Rpc Error: Failed to send response header");
            self->_logger.Error("Rpc Error: Failed to send response header");
            break;
        }
        
        attempts = 0;
        do {
            sendResult = send(*msgsock, response.data(), response.size(), 0);
        } while (sendResult == -1 && ++attempts < self->_retry);

        if (sendResult == -1) {
            perror("Rpc Error: Failed to send response body");
            self->_logger.Error("Rpc Error: Failed to send response body");
            break;
        }
    }

    close(*msgsock);
    delete msgsock;
    return nullptr;
}
