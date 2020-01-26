#ifndef FRONTIERRANK_H
#define FRONTIERRANK_H

#include "src/libs/parsedurl/ParsedUrl.h"

namespace BBG {


    string getRoot(const string& url);

    bool protocolIsHttps(const string& url);

    bool extensionIsGood(const string& url);

    bool urlIsGood(const string& url);

}

#endif
