#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "src/libs/map/map.h"
#include "src/libs/serialize/Buffer.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"

namespace BBG {
class Document {
 public:
  Document();
  Document(const char* url);
  Document(string& url);

  string url;
  vector<string> titleText;
  vector<string> bodyText;
  Map<string, vector<string>> anchors;
  size_t imgCount;
  size_t size;

  Document& operator=(const Document& right);
  bool operator==(const Document& other) const;
  bool operator!=(const Document& other) const;
  
  vector<string> getFilteredUrls();
  void clear();

  friend std::ostream& operator<<(std::ostream& os, Document& str);
  friend void serialize(const Document& doc, Buffer& bytes);
  friend void deserialize(Buffer& bytes, Document& doc);
};

std::ostream& operator<<(std::ostream& os, Document& str);
void serialize(const Document& doc, Buffer& bytes);
void deserialize(Buffer& bytes, Document& doc);
}  // namespace BBG

#endif
