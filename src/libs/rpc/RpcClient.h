#ifndef RPC_CLIENT_H
#define RPC_CLIENT_H

#include "src/libs/parsedurl/ParsedUrl.h"
#include "src/libs/string/string.h"
#include "src/libs/serialize/Buffer.h"
#include "src/libs/logger/logger.h"
#include "netdb.h"

namespace BBG {
    class RpcClient {
    public:
        // Opens a connection and reuses it
        RpcClient(const char* url);
        RpcClient(string url);
        RpcClient(ParsedUrl url);

        // Send a request and wait for a response returning the serialized response
        Buffer request(const char* keyword, const Buffer& requestBytes);
        Buffer request(const string& keyword, const Buffer& requestBytes);

        void requestNonBlocking(const char* keyword, const Buffer& requestBytes);
        void requestNonBlocking(const string& keyword, const Buffer& requestBytes);

        ~RpcClient();

    private:
        ParsedUrl _url;
        int _sock;
        addrinfo *_address;
        static const int _retry = 5;
        Logger _logger;

        void initializeSocket();
        void resolveDNS();
        void createSocket();
        void connectToServer();
    };
}

#endif