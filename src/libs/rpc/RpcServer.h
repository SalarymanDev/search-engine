#ifndef RPC_SERVER_H
#define RPC_SERVER_H

#include "src/libs/string/string.h"
#include "src/libs/logger/logger.h"
#include "src/libs/serialize/Buffer.h"
#include "src/libs/map/map.h"
#include "src/libs/server/Server.h"

// Request Formats: <keyword> <b|n> <binary>
// Examples:        post_urls ["https://google.com"]
//                  get_urls

// Response Format: <binary>
// Example:         ["https://google.com"]
// Error Example:   { "code": 500, "msg": "This is an error!" }
// Errors are of type ErrorResponse

namespace BBG {
    class RpcServer : public Server {
    public:
        RpcServer(int port, int listenQueueSize = 25, int retry = 5);

        // Endpoint Function format (returns Buffer of bytes):
        // Buffer endpointExample(Buffer& requestBytes);
        void registerEndpoint(string keyword, Buffer (*handler)(Buffer&));
    
    private:
        Map<string, Buffer (*)(Buffer&)> _endpoints;

        // Listener thread receives incoming requests
        // and spawns a requestHandler thread passing
        // it the msgsock for the session.
        static void* requestHandler(void* arg);
    };
}

#endif