#include "src/libs/document/Document.h"
#include "src/libs/parsedurl/ParsedUrl.h"
#include <string>

using BBG::Buffer;
using BBG::Document;
using BBG::Map;
using BBG::string;
using BBG::vector;
using BBG::ParsedUrl;
using BBG::strlen;

Document::Document() : imgCount(0), size(0) {}

Document::Document(const char *_url) : url(_url), imgCount(0), size(0) {}

Document::Document(string &_url) : url(_url), imgCount(0), size(0) {}

void Document::clear() {
  url.clear();
  titleText.clear();
  bodyText.clear();
  anchors.clear();
  imgCount = 0;
}

Document &Document::operator=(const Document &right) {
  url = right.url;
  titleText = right.titleText;
  bodyText = right.bodyText;
  anchors = right.anchors;
  imgCount = right.imgCount;
  size = right.size;
  return *this;
}

bool Document::operator==(const Document &other) const {
  if (url != other.url) return false;
  if (titleText != other.titleText) return false;
  if (bodyText != other.bodyText) return false;
  if (anchors != other.anchors) return false;
  if (imgCount != other.imgCount) return false;
  if (size != other.size) return false;
  return true;
}

bool Document::operator!=(const Document &other) const {
  return !(*this == other);
}

bool urlIsGood(string &url) {
  ParsedUrl parsedUrl(url);
  char *ptr = parsedUrl.Host + strlen(parsedUrl.Host) - 1;
  while (*ptr && *ptr != '.') --ptr;
  string rootDomain(ptr);
  
  if (rootDomain == ".com")
    return true;
  if (rootDomain == ".gov")
    return true;
  if (rootDomain == ".edu")
    return true;
  if (rootDomain == ".net")
    return true;
  if (rootDomain == ".org")
    return true;
  if (rootDomain == ".uk")
    return true;
  if (rootDomain == ".ca")
    return true;

  return false;
}

vector<string> Document::getFilteredUrls() {
  vector<string> urls;
  urls.reserve(anchors.size());
  for (auto e = anchors.begin(); e; e = anchors.next()) {
    if (urlIsGood(e->key))
      urls.push_back(e->key);
  }
  return urls;
}

std::ostream &BBG::operator<<(std::ostream &os, Document &doc) {
  size_t i = 0;
  os << "url: " << doc.url << "\n";

  os << "title text:";
  for (i = 0; i < doc.titleText.size(); ++i) os << " " << doc.titleText[i];

  os << "\nbody text:";
  for (i = 0; i < doc.bodyText.size(); ++i) os << " " << doc.bodyText[i];

  os << "\nanchor urls:\n";
  for (auto iter = doc.anchors.begin(); iter; iter = doc.anchors.next()) {
    os << iter->key << " :";
    for (i = 0; i < iter->val.size(); ++i) os << " " << iter->val[i];
    os << "\n";
  }

  os << "img count: " << doc.imgCount << "\n";

  os << "doc size: " << doc.size << "\n";

  return os;
}

void BBG::serialize(const Document &doc, Buffer &bytes) {
  serialize('\n', bytes);
  serialize(doc.url, bytes);
  serialize(doc.titleText, bytes);
  serialize(doc.bodyText, bytes);
  serialize(doc.anchors, bytes);
  serialize(doc.imgCount, bytes);
  serialize(doc.size, bytes);
  serialize('\n', bytes);
}

void BBG::deserialize(Buffer &bytes, Document &doc) {
  char trash;
  deserialize(bytes, trash);
  deserialize(bytes, doc.url);
  deserialize(bytes, doc.titleText);
  deserialize(bytes, doc.bodyText);
  deserialize(bytes, doc.anchors);
  deserialize(bytes, doc.imgCount);
  deserialize(bytes, doc.size);
  deserialize(bytes, trash);
}
