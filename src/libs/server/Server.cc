#include "src/libs/server/Server.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "netdb.h"
#include <string.h>
#include <unistd.h>
#include <iostream>

using BBG::Server;
using std::cout;
using std::endl;

Server::Server(int port, void *(handler)(void *), int listenQueueSize, int retry)
    : _port(port),
      _listenQueueSize(listenQueueSize),
      _retry(retry),
      _handler(handler) {}

void Server::run() {
    initializeSocket();
    listen(_sock, _listenQueueSize);
    pthread_create(&_listener, nullptr, static_cast<void *(*)(void *)>(&BBG::Server::listener), this);
}

void Server::stop() {
    pthread_cancel(_listener);
    close(_sock);
    _sock = -1;
}

void Server::join() {
    pthread_join(_listener, nullptr);
}

Server::~Server() {
    if (_sock != -1)
        stop();
}

void Server::initializeSocket() {
    _sock = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (_sock == -1) {
        perror("Rpc Error: opening stream socket");
        _logger.Fatal("Rpc Error: opening stream socket.");
    }

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = INADDR_ANY;
    sock_addr.sin_port = htons(_port);

    if (bind(_sock, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) == -1) {
        perror("Rpc Error: Failed to bind socket");
        _logger.Fatal("Rpc Error: Failed to bind socket");
    }

    socklen_t length = sizeof(sock_addr);
    if (getsockname(_sock, (struct sockaddr *) &sock_addr, &length) == -1) {
        perror("Rpc Error: Failed to get socket name");
        _logger.Fatal("Rpc Error: Failed to get socket name");
    }

    _port = ntohs(sock_addr.sin_port);
    cout << "Server bound on port: " << _port << endl;
}

void *Server::listener(void *arg) {
    Server *self = static_cast<Server*>(arg);

    struct sockaddr_in talkAddress;
    socklen_t  talkAddressLength;

    memset( &talkAddress, 0, sizeof( talkAddress ) );

    do {
        int *msgsock = new int(-1);
        *msgsock = accept(self->_sock, (sockaddr*)&talkAddress, &talkAddressLength);
        if (*msgsock == -1) {
            perror("Rpc Error: Failed to accept connection");
            self->_logger.Error("Rpc Error: Failed to accept connection");
            continue;
        }
        pthread_t newThread;
        args_struct *args = new args_struct(msgsock, self);
        pthread_create(&newThread, nullptr, self->_handler, static_cast<void *>(args));
        pthread_detach(newThread);
    } while (1);

    return nullptr;
}