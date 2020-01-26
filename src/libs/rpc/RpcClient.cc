#include "src/libs/rpc/RpcClient.h"
#include "sys/types.h"
#include "sys/socket.h"
#include <iostream>
#include <unistd.h>
#include <cstring>

using BBG::RpcClient;
using BBG::ParsedUrl;
using BBG::vector;
using BBG::string;
using BBG::Buffer;
using std::cout;
using std::endl;

RpcClient::RpcClient(const char* url) : _url(url) {
    initializeSocket();
}

RpcClient::RpcClient(BBG::string url) : _url(url) {
    initializeSocket();
}

RpcClient::RpcClient(ParsedUrl url) : _url(url) {
    initializeSocket();
}

void RpcClient::initializeSocket() {
    resolveDNS();
    createSocket();
    connectToServer();
}

Buffer RpcClient::request(const char* keyword, const Buffer& requestBytes) {
    string keywordString(keyword);
    return request(keywordString, requestBytes);
}

void RpcClient::requestNonBlocking(const char* keyword, const Buffer& requestBytes) {
    string keywordString(keyword);
    return requestNonBlocking(keywordString, requestBytes);
}

Buffer RpcClient::request(const string& keyword, const Buffer& requestBytes) {
    // Construct request
    Buffer request(keyword.c_str(), keyword.size());
    request += " b ";
    request += requestBytes;

    // Send header
    size_t size = request.size();
    ssize_t sendResult;
    int attempts = 0;
    do {
        sendResult = send(_sock, &size, sizeof(size_t), 0);
    } while (sendResult == -1 && ++attempts < _retry);

    if (sendResult == -1) {
        perror("RpcClient Error: Failed to send request header");
        _logger.Error("RpcClient Error: Failed to send request header");
        return Buffer("Error: Failed to send msg to server", 36);
    }

    // Send request
    attempts = 0;
    do {
        sendResult = send(_sock, request.data(), request.size(), 0);
    } while (sendResult == -1 && ++attempts < _retry);

    if (sendResult == -1) {
        perror("RpcClient Error: Failed to send request body");
        _logger.Error("RpcClient Error: Failed to send request body");
        return Buffer("Error: Failed to send msg to server", 36);
    }

    // Get response header
    size_t responseSize;
    recv(_sock, &responseSize, sizeof(size_t), MSG_WAITALL);

    // Get response
    char buff[responseSize];
    recv(_sock, buff, responseSize, MSG_WAITALL);
    return Buffer(buff, responseSize);
}

void RpcClient::requestNonBlocking(const string& keyword, const Buffer& requestBytes) {
    // Construct request
    Buffer request(keyword.c_str(), keyword.size());
    request += " n ";
    request += requestBytes;

    // Send header
    size_t size = request.size();
    ssize_t sendResult;
    int attempts = 0;
    do {
        sendResult = send(_sock, &size, sizeof(size_t), 0);
    } while (sendResult == -1 && ++attempts < _retry);

    if (sendResult == -1) {
        perror("RpcClient Error: Failed to send request header");
        _logger.Error("RpcClient Error: Failed to send request header");
        return;
    }

    // Send request
    attempts = 0;
    do {
        sendResult = send(_sock, request.data(), request.size(), 0);
    } while (sendResult == -1 && ++attempts < _retry);

    if (sendResult == -1) {
        perror("RpcClient Error: Failed to send request body");
        _logger.Error("RpcClient Error: Failed to send request body");
        return;
    }
}

RpcClient::~RpcClient() {
    close(_sock);
    freeaddrinfo(_address);
}

void RpcClient::resolveDNS() {
    struct addrinfo hints;
    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int getaddrResult, attempts = 0;
    do {
        getaddrResult = getaddrinfo( _url.Host, *_url.Port ? _url.Port : "80", &hints, &_address );
        ++attempts;
    } while (getaddrResult == -1 && attempts < _retry);

    if (getaddrResult < 0) {
        perror("Rpc Error: Failed to resolve DNS");
        _logger.Fatal("Rpc Error: Failed to resolve DNS");
    }
}

void RpcClient::createSocket() {
    int attempts = 0;
    do {
        _sock = socket( _address->ai_family, _address->ai_socktype, _address->ai_protocol );
        ++attempts;
    } while (_sock == -1 && attempts < _retry);

    if (_sock == -1) {
        perror("Rpc Error: Failed to get socket");
        _logger.Fatal("Rpc Error: Failed to get socket");
    }
}

void RpcClient::connectToServer() {
    int attempts = 0;
    ssize_t connectResult;
    do {
        connectResult = connect(_sock, _address->ai_addr, sizeof(struct sockaddr));
        ++attempts;
    } while (connectResult == -1 && attempts < _retry);

    if (connectResult == -1) {
        perror("Rpc Error: Failed to connect to server");
        _logger.Fatal("Rpc Error: Failed to connect to server");
    }
}
