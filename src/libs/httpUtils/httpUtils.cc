#include "httpUtils.h"
#include "src/libs/connection/connection.h"
#include "src/libs/logger/logger.h"
#include "src/libs/string/string.h"
#include "src/libs/utils/constants.h"
#include "src/libs/utils/utils.h"
#include "src/libs/vector/vector.h"

#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>

#include <openssl/ssl.h>

using namespace BBG;

constexpr int HttpUtil::MaxRedirects;
constexpr int HttpUtil::PageBufferSize;
constexpr char HttpUtil::UserAgent[];

HttpUtil::ReturnCode HttpUtil::GetPage(vector<string>& crawledUrls,
                                       string& out) {
  return getPageImpl(crawledUrls, out, ctx, &log);
}

HttpUtil::ReturnCode HttpUtil::getPageImpl(vector<string>& crawledUrls,
                                           string& out, SSL_CTX* ctx,
                                           Logger* log) {
  int err;
  ReturnCode ret = HttpUtil::Redirected;

  // Follows up to 10 redirects
  for (int count = 0; count < 10; ++count) {
    Page page(crawledUrls.back().c_str());
    Connection conn;

    char count_buf[32];
    sprintf(count_buf, "%u", count);
    log->Debug("[HttpUtils] Redirect attempt " + string(count_buf));
    // Connect with ssl or not based on parsed service
    err = conn.connect(page.host, ::atoi(page.port),
                       (page.service[4] == 's') ? ctx : nullptr);

    if (err) return SystemError;

    sendHTTPGet(conn, page);

    ret = parseHTTPResponse(conn, out, log);
    if (ret == Redirected)
      crawledUrls.push_back(out);
    else
      break;
  }
  // log->Debug("[HttpUtils] returning from getPage impl with resp:\n" + out);
  return ret;
}

void HttpUtil::sendHTTPGet(const Connection& connection, const Page& page) {
  int hdrConst = 90;
  int uagentLen = ::strlen(UserAgent);
  int hostLen = ::strlen(page.host);
  int pathLen = ::strlen(page.path);
  int hdrLen = hdrConst + uagentLen + hostLen + pathLen;

  char hdr[hdrLen];

  snprintf(hdr, hdrLen,
           "GET /%s HTTP/1.1\r\n"
           "User-Agent: %s\r\n"
           "Host: %s\r\n"
           "Accept: text/*\r\n"
           "Connection: close\r\n\r\n",
           page.path, UserAgent, page.host);
  connection.Write(hdr, hdrLen);
}

HttpUtil::ReturnCode HttpUtil::parseHTTPResponse(const Connection& conn,
                                                 string& out, Logger* log) {
  int bytes = 1;
  char buff[10 * 1024];
  size_t BodyStartPos = 0;

  string resp;

  // Read loop from connection
  while (bytes > 0) {
    bytes = conn.Read(buff, sizeof(buff));
    resp.append(buff, bytes);
  }
  // Try to find the end of the headers
  BodyStartPos = resp.find("\r\n\r\n", 0);

  // We did not get a proper HTTP response or we got no response
  if (BodyStartPos == string::npos) return MalformedHeader;

  // Mark the headers as a c string
  resp[BodyStartPos] = '\0';
  BodyStartPos += 4;

  // Get Status Code
  int status;
  int err = sscanf(resp.c_str(), "HTTP/1.1 %d", &status);
  if (err == 0) return MalformedHeader;

  // Look for redirects
  char* p;

  // TODO: Performance optimization possible, we don't need to use the resp
  // after this point
  char tmp[resp.size()];
  char location[MaxUrlLength];
  char contentType[MaxMimeLength];
  for (size_t i = 0; i < resp.size(); ++i) tmp[i] = resp[i];

  // Tokenize response headers, looking for redirect location
  p = strtok(tmp, "\r\n");
  while (p != NULL) {
    // Server returned something other than text
    if (sscanf(p, "Content-Type: %s", contentType) &&
        !strncmp(contentType, "text/", 4))
      return MalformedHeader;

    // Redirect
    if (sscanf(p, "Location: %s", location)) {
      out = location;
      // Log and return redirected
      char redirectMsg[13 + out.size()];
      sprintf(redirectMsg, "Redirected to %s\n", location);
      //log->Error(redirectMsg);

      return Redirected;
    }

    p = strtok(NULL, "\r\n");
  }

  // If a page wants us to use a proxy, we don't need that page
  if (status == 305) return BadResponse;

  // Status: 4**
  if (status >= 400 && status < 500) return BadResponse;

  // Status: 5**
  if (status >= 500 && status < 600) return BadResponse;

  // Write to output string
  out = resp.substr(BodyStartPos, resp.size() - BodyStartPos);
  return Ok;
}
