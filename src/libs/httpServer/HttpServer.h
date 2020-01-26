#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "src/libs/server/Server.h"
#include "src/libs/map/map.h"
#include "src/libs/string/string.h"
#include "src/libs/httpServer/HttpResponse.h"
#include "src/libs/httpServer/HttpRequest.h"
#include "src/libs/serialize/Buffer.h"

namespace BBG {
    class HttpServer : public Server {
    public:
        HttpServer(string wwwrootPath, int port, int listenQueueSize = 25, int retry = 5);

        // Endpoint Function format (returns an HttpResponse object):
        // string endpointExample(string& requestBytes);
        void registerEndpoint(string path, string method, HttpResponse *(*handler)(HttpRequest&));
    private:
        struct StaticFile {
            string content;
            string mimeType;
        };

        string wwwrootPath;

        Map<string, HttpResponse *(*)(HttpRequest&)> _endpoints;
        Map<string, StaticFile> _staticFiles;

        void fileSearch(string &dirPath);
        string Mimetype(const string& filename);
        size_t FileSize(int fd);

        // Listener thread receives incoming requests
        // and spawns a requestHandler thread passing
        // it the msgsock for the session.
        static void* requestHandler(void* arg);
    };

    struct MimetypeMap {
        const char *Extension, *Mimetype;
    };

    const MimetypeMap MimeTable[ ] = {
        {".aac",     "audio/aac"},
        {".abw",     "application/x-abiword"},
        {".arc",     "application/octet-stream"},
        {".avi",     "video/x-msvideo"},
        {".azw",     "application/vnd.amazon.ebook"},
        {".bin",     "application/octet-stream"},
        {".bz",      "application/x-bzip"},
        {".bz2",     "application/x-bzip2"},
        {".csh",     "application/x-csh"},
        {".css",     "text/css"},
        {".csv",     "text/csv"},
        {".doc",     "application/msword"},
        {".docx",    "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
        {".eot",     "application/vnd.ms-fontobject"},
        {".epub",    "application/epub+zip"},
        {".gif",     "image/gif"},
        {".htm",     "text/html"},
        {".html",    "text/html"},
        {".ico",     "image/x-icon"},
        {".ics",     "text/calendar"},
        {".jar",     "application/java-archive"},
        {".jpeg",    "image/jpeg"},
        {".jpg",     "image/jpeg"},
        {".js",      "application/javascript"},
        {".map",      "application/javascript"},
        {".json",    "application/json"},
        {".mid",     "audio/midi"},
        {".midi",    "audio/midi"},
        {".mpeg",    "video/mpeg"},
        {".mpkg",    "application/vnd.apple.installer+xml"},
        {".odp",     "application/vnd.oasis.opendocument.presentation"},
        {".ods",     "application/vnd.oasis.opendocument.spreadsheet"},
        {".odt",     "application/vnd.oasis.opendocument.text"},
        {".oga",     "audio/ogg"},
        {".ogv",     "video/ogg"},
        {".ogx",     "application/ogg"},
        {".otf",     "font/otf"},
        {".png",     "image/png"},
        {".pdf",     "application/pdf"},
        {".ppt",     "application/vnd.ms-powerpoint"},
        {".pptx",    "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
        {".rar",     "application/x-rar-compressed"},
        {".rtf",     "application/rtf"},
        {".sh",      "application/x-sh"},
        {".svg",     "image/svg+xml"},
        {".swf",     "application/x-shockwave-flash"},
        {".tar",     "application/x-tar"},
        {".tif",     "image/tiff"},
        {".tiff",    "image/tiff"},
        {".ts",      "application/typescript"},
        {".ttf",     "font/ttf"},
        {".vsd",     "application/vnd.visio"},
        {".wav",     "audio/x-wav"},
        {".weba",    "audio/webm"},
        {".webm",    "video/webm"},
        {".webp",    "image/webp"},
        {".woff",    "font/woff"},
        {".woff2",   "font/woff2"},
        {".xhtml",   "application/xhtml+xml"},
        {".xls",     "application/vnd.ms-excel"},
        {".xlsx",    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
        {".xml",     "application/xml"},
        {".xul",     "application/vnd.mozilla.xul+xml"},
        {".zip",     "application/zip"},
        {".3gp",     "video/3gpp"},
        {".3g2",     "video/3gpp2"},
        {".7z",      "application/x-7z-compressed}"}
    };
}

#endif
