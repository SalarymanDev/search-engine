#pragma once

#include "src/libs/logger/logger.h"

#include <cstdlib>
#include <openssl/ssl.h>

namespace BBG {
  class Connection {
    public:
    Connection();
    ~Connection();
    int connect(const char* domain, int port);
    int connect(const char* domain, int port, SSL_CTX* ctx);

    int Read(char* buff, int len) const;
    int Write(const char* buff, int len) const;

    private:
    int socket;
    SSL* ssl;
    Logger log;
  };
}
