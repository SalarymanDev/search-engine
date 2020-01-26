#pragma once

#include <openssl/ssl.h>
#include <cstring>

#include "page.h"
#include "src/libs/connection/connection.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/logger/logger.h"

namespace BBG{
  class HttpUtil{
    /*
    * A function library for crawling pages.
    */

    constexpr static int MaxRedirects = 10;
    constexpr static int PageBufferSize = 10*1024;
    constexpr static char UserAgent[] = "LinuxGetSSL/2.0 boisboisgo@umich.edu (Linux)";

    Logger log;
    SSL_CTX* ctx;

    public:

    // Retun Codes
    enum ReturnCode{
      Ok = 0,
      Redirected,
      MalformedHeader,
      BadResponse,
      SystemError,
    };

    HttpUtil() 
      : ctx(SSL_CTX_new(TLS_method())) {}

    ~HttpUtil() {
      SSL_CTX_free(ctx);
    }

    /*
    * getPage will download the url and write it to the passed
    * in string as it parses it
    *
    * HTTPGet will follow up to 10 redirects.
    *
    * @param visitedUrls is a vector all urls the function hits in order to
    * download the page. Both getHTTPPage and getPage assume visitiedUrls[0]
    * contains the url to download. We folllow 10 redirects before marking as a
    * loop and returning.
    * @param out contains html the output of a page
    * @returns 0 upon success, -1 upon 305/4xx/5xx, 3 upon 30[1-4,7]
    */
    ReturnCode GetPage(vector<string>& visitiedUrls, string& out);

    private:
    //--------------------------------Helpers-------------------------------------

    static ReturnCode getPageImpl(vector<string>& visitiedUrls, string& out,
        SSL_CTX* ctx, Logger* log);

    static void sendHTTPGet(const Connection& connection, const Page& page);

    static ReturnCode parseHTTPResponse(const Connection& connection, string& out,
        Logger* log);
  }; //httpUtils
}; //namespace BBG
