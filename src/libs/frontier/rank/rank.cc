#include "src/libs/frontier/rank/rank.h"
#include "src/libs/string/string.h"

using BBG::string;

const string ProtocolEnd = "://";
const size_t ProtocolEndSize = ProtocolEnd.size();
const char Slash = '/';


string BBG::getRoot(const string& url) {
   size_t host_start = url.find(ProtocolEnd);
   size_t host_end = url.find(Slash, host_start + ProtocolEndSize);
   string res;

   if (host_start != string::npos)
      res = url.substr(host_start + ProtocolEndSize, host_end - host_start - ProtocolEndSize);
   return res;
}


bool BBG::protocolIsHttps(const string& url) {
    return strncmp(url.c_str(), "https", 5);
}


bool BBG::extensionIsGood(const string& url) {
    
    string root = getRoot(url);
    string ext = root.substr(root.size() - 3, 3);

    return ext == "org" || ext == "edu" 
        || ext == "gov" || ext == "com";
}


// TODO: Extend the ranking criteria of this function to be better
bool BBG::urlIsGood(const string& url) {
    return url.size() < 256 
        && extensionIsGood(url) 
        && url.contains('/') < 7;
        //&& protocolIsHttps(url);
}
