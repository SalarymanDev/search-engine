#ifndef SERVER_H
#define SERVER_H

#include "src/libs/logger/logger.h"

namespace BBG {
    class Server {
    protected:
        pthread_t _listener = 0;
        int _sock;
        int _port;
        int _listenQueueSize;
        Logger _logger;

        void initializeSocket();

        // Listener thread receives incoming requests
        // and spawns a requestHandler thread passing
        // it the msgsock for the session.
        static void *listener(void *arg);
        void *(*_handler)(void *);

        const int _retry;

        struct args_struct {
            int *msgsock;
            Server *self;

            args_struct(int *msgsock, Server *self) :
                    msgsock(msgsock),
                    self(self) {}
        };

    public:
        Server(int port, void *(*handler)(void *), int listenQueueSize, int retry);

        void run();
        void stop();
        void join();

        ~Server();
    };
}

#endif