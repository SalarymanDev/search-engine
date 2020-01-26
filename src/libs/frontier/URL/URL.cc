#include "src/libs/frontier/URL/URL.h"

using BBG::URL;

URL::URL() {}

URL::URL(const string& _url, const string & _root) :
    url(_url), root(_root) {}

URL& URL::operator=(const URL& other) {
    if (this != &other) {
        url = other.url;
        root = other.root;
    }
    return *this;
}

void BBG::swap(URL &a, URL &b) {
    BBG::swap(a.root, b.root);
    BBG::swap(a.url, b.url);
}