#include "connection.h"
#include "src/libs/logger/logger.h"

#include "openssl/ssl.h"

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

using BBG::Connection;

Connection::Connection() : socket(0), ssl(nullptr) {}

int Connection::connect(const char* domain, int port) {
  return connect(domain, port, nullptr);
};

int Connection::connect(const char* domain, int port, SSL_CTX* ctx) {
  struct addrinfo hints, *result;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  char port_str[10];
  sprintf(port_str, "%d", port);
  int err = getaddrinfo(domain, port_str, &hints, &result);
  if (err) {
    // log.Error("[Connection] getting addrinfo:");
    // log.Error(string(gai_strerror(err)));
    return -1;
  }

  socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

  if (socket < 0) {
    log.Error("Error getting socket");
    return -1;
  }

  // Set socket timeout to 2 second
  struct timeval tv;
  tv.tv_sec = 2;
  tv.tv_usec = 0;

  setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
  setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

  // Set sock to nonblocking for connect call
  long arg = fcntl(socket, F_GETFL, NULL) | O_NONBLOCK;
  fcntl(socket, F_SETFL, arg);

  if (!port) port = (ctx) ? 443 : 80;

  ((sockaddr_in*)result->ai_addr)->sin_port = htons(port);
  err = ::connect(socket, result->ai_addr, result->ai_addrlen);
  freeaddrinfo(result);

  fd_set timeoutSet;
  FD_ZERO(&timeoutSet);
  FD_SET(socket, &timeoutSet);
  err = ::select(socket + 1, NULL, &timeoutSet, NULL, &tv);
  if (err <= 0) {
    char errMsg[strlen(domain) + 22];
    // sprintf(errMsg, "Error connecting to %s", domain);
    // log.Error(errMsg);
    return -1;
  }

  // Set sock back to blocking for recv call
  arg = fcntl(socket, F_GETFL, NULL) & ~O_NONBLOCK;
  fcntl(socket, F_SETFL, arg);

  // Set up SSL if necessary
  if (ctx) {
    ssl = SSL_new(ctx);
    SSL_set_fd(ssl, socket);
    SSL_connect(ssl);
  } else {
    ssl = nullptr;
  }
  return 0;
};

Connection::~Connection() {
  if (ssl != nullptr) {
    SSL_shutdown(ssl);
    SSL_free(ssl);
  }
  ::close(socket);
}

int Connection::Read(char* buff, int len) const {
  if (ssl != nullptr)
    return SSL_read(ssl, buff, len);
  else
    return recv(socket, buff, len, 0);
}

int Connection::Write(const char* buff, int len) const {
  if (ssl != nullptr)
    return SSL_write(ssl, buff, len);
  else
    return send(socket, buff, len, MSG_NOSIGNAL);
}
