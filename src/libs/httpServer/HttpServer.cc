#include "src/libs/httpServer/HttpServer.h"
#include "src/libs/utils/utils.h"
#include "sys/types.h"
#include "sys/socket.h"
#include <fcntl.h>
#include <sys/stat.h>
#include "netdb.h"
#include <unistd.h>
#include <iostream>
#include <dirent.h>

using BBG::HttpServer;
using BBG::HttpResponse;
using BBG::HttpRequest;
using BBG::string;
using BBG::Map;
using BBG::Buffer;
using std::cout;
using std::endl;

HttpServer::HttpServer(string wwwrootPath, int port, int listenQueueSize, int retry)
    : Server(port, (void *(*)(void *))&HttpServer::requestHandler, listenQueueSize, retry),
      wwwrootPath(wwwrootPath) {
    fileSearch(wwwrootPath);
}

void HttpServer::fileSearch(string &dirPath) {
    DIR *dir = opendir(dirPath.c_str());
    if (!dir)
        _logger.Fatal("[HttpServer:FileSearch] Unable to open folder: " + dirPath);

    dirent* entry;
    while ((entry = readdir(dir))) {
        string filename(entry->d_name);
        string newPath(dirPath + "/");
        newPath += filename;

        if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {
            int fd = open(newPath.c_str(), O_RDONLY);
            if (fd == -1) {
                _logger.Error("[HttpServer:FileSearch] Unable to open file: " + newPath);
                continue;
            }

            _logger.Info("[HttpServer:FileSearch] Cached file: " + newPath);

            size_t filesize = FileSize(fd);
            Buffer fileContent(filesize);
            ssize_t result = read(fd, fileContent.modifyData(), filesize);
            if (result == -1) {
                _logger.Error("[HttpServer:FileSearch] Unable to read file: " + newPath);
                continue;
            }

            string content(fileContent.data(), fileContent.size());
            string pathWithoutRoot(newPath.c_str() + wwwrootPath.size() + 1);
            _staticFiles["/" + pathWithoutRoot] = {
                content, Mimetype(filename)
            };
        } else if (entry->d_type == DT_DIR) {
            if (entry->d_name[0] == '.')
                continue;
            fileSearch(newPath);
        }
    }

    closedir(dir);
}

size_t HttpServer::FileSize(int fd) {
    struct stat fileInfo;
    fstat(fd, &fileInfo);
    return fileInfo.st_size;
}

void HttpServer::registerEndpoint(string path, string method, HttpResponse *(*handler)(HttpRequest&)) {
    if (_listener != 0)
        _logger.Fatal("HttpServer Error: Cannot register endpoints after starting server.");

    string key(path);
    key += " ";
    key += method;
    _endpoints[key] = handler;
}

void closeConnection(int *msgsock) {
    close(*msgsock);
    delete msgsock;
}

string generateLog(HttpRequest &request, HttpResponse *response) {
    string msg;
    char buff[20];
    sprintf(buff, "%zu", (size_t)response->status);

    msg += request.method;
    msg += " ";
    msg += buff;
    msg += " ";
    msg += request.path;

    return msg;
}

void* HttpServer::requestHandler(void* arg) {
    args_struct *args = static_cast<args_struct*>(arg);
    int *msgsock = args->msgsock;
    HttpServer *self = static_cast<HttpServer*>(args->self);
    delete args;

    size_t bytes = 0;
    char buff[10*1024];
    bytes = recv(*msgsock, buff, sizeof(buff), 0);
    string requestString(buff, bytes);

    // Check if client closed connection;
    if (requestString.empty()) {
        closeConnection(msgsock);
        return nullptr;
    }

    HttpRequest request(requestString);
    if (request.path == "/")
        request.path = "/index.html";

    HttpResponse *response;
    if (!request.isValid) {
        // Check if request is valid
        response = new HttpResponse();
        response->status = HttpResponse::StatusCode::BAD_REQUEST;
    } else if (self->_endpoints.contains(request.endpointKey())) {
        // Check if endpoint exists
        HttpResponse *(*endpoint)(HttpRequest&) = *(self->_endpoints[request.endpointKey()]);
        response = endpoint(request);

    } else if (self->_staticFiles.contains(request.path)) {
        // Check if static file exists
        response = new HttpResponse();
        response->status = HttpResponse::StatusCode::OK;
        response->headers["Content-Type"] = self->_staticFiles[request.path].mimeType;
        string &content = self->_staticFiles[request.path].content;
        response->body = content;
    } else {
        // Fall back on 404 not found
        response = new HttpResponse();
        response->status = HttpResponse::StatusCode::NOT_FOUND;
    }

    response->headers["Connection"] = "Closed";

    self->_logger.Info(generateLog(request, response));

    string responseString = response->toString();
    delete response;

    int attempts = 0;
    ssize_t sendResult;
    do {
        sendResult = send(*msgsock, responseString.c_str(), responseString.size(), 0);
    } while (sendResult == -1 && ++attempts < self->_retry);

    if (sendResult == -1) {
        perror("Rpc Error: Failed to send response body");
        self->_logger.Error("Rpc Error: Failed to send response body");
        closeConnection(msgsock);
        return nullptr;
    }

    closeConnection(msgsock);
    return nullptr;
}

string HttpServer::Mimetype(const string& filename) {
    size_t start = filename.size();
    while (start > 0 && filename.c_str()[start] != '.')
        --start;
    if (start == 0) return "application/octet-stream";

    for (size_t i = 0; i < sizeof(MimeTable); ++i) {
        if (strcmp(MimeTable[i].Extension, filename.c_str() + start)) {
            return MimeTable[i].Mimetype;
        }
    }

    return "application/octet-stream";
}
