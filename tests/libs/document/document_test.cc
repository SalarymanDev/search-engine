#include "src/libs/document/Document.h"
#include "tests/catch.hpp"
#include <fstream>
#include <string>

using BBG::Buffer;
using BBG::deserialize;
using BBG::Document;
using BBG::serialize;
using BBG::string;
using BBG::vector;

TEST_CASE("Constructor", "[Document]") {
  string expected = "http://example.com";
  Document doc(expected);
}

TEST_CASE("Compare should be true", "[Document]") {
  string title("title");
  string body("body");
  string anchorUrl("anchorUrl");
  string anchor("anchor");

  Document left("testurl");
  left.titleText.push_back(title);
  left.bodyText.push_back(body);
  left.anchors[anchorUrl].push_back(anchor);

  Document right("testurl");
  right.titleText.push_back(title);
  right.bodyText.push_back(body);
  right.anchors[anchorUrl].push_back(anchor);

  REQUIRE(left == right);
}

TEST_CASE("Compare should be false", "[Document]") {
  string title("title");
  string body("body");
  string anchorUrl("anchorUrl");
  string anchor("anchor");

  Document left("testurl");
  left.titleText.push_back(title);
  left.bodyText.push_back(body);
  left.anchors[anchorUrl].push_back(anchor);

  Document right("testurl");
  right.titleText.push_back(body);
  right.bodyText.push_back(title);
  right.anchors[anchorUrl].push_back(anchor);

  REQUIRE(left != right);
}

TEST_CASE("Assignment operator", "[Document]") {
  string title("title");
  string body("body");
  string anchorUrl("anchorUrl");
  string anchor("anchor");

  Document expected("testurl");
  expected.titleText.push_back(title);
  expected.bodyText.push_back(body);
  expected.anchors[anchorUrl].push_back(anchor);

  Document actual("testurl");
  actual = expected;

  REQUIRE(expected == actual);
}

TEST_CASE("getFilteredUrls", "[Document]") {
  Document doc;
  string token = "token";

  doc.anchors["https://www.google.com/"].push_back(token);
  doc.anchors["https://umich.edu/"].push_back(token);
  doc.anchors["https://google.co.uk/"].push_back(token);
  doc.anchors["https://google.gov/"].push_back(token);
  doc.anchors["https://google.net/"].push_back(token);
  doc.anchors["https://google.org/"].push_back(token);
  doc.anchors["https://google.ca/"].push_back(token);
  doc.anchors["https://google.xyz/"].push_back(token);
  doc.anchors["https://google.ru/"].push_back(token);

  vector<string> gotUrls = doc.getFilteredUrls();
  REQUIRE(gotUrls.size() == 7);
}

TEST_CASE("clear", "[Document]") {
  string exampleUrl = "https://example.com";
  string title = "example";
  string body = "<html><body>Test</body></html>";
  Document dirty(exampleUrl);
  dirty.titleText.push_back(title);
  dirty.bodyText.push_back(body);
  dirty.anchors[exampleUrl].push_back(title);

  Document clean;

  REQUIRE(dirty != clean);
  dirty.clear();
  REQUIRE(dirty == clean);
}

// TODO: Waiting on our map
TEST_CASE("Serialization and deserialization.", "[Document]") {
  string hello("Hello");
  string world("World!");
  string googleUrl("https://google.com/");

  Document expected("http://test.com");
  expected.titleText.push_back(hello);
  expected.titleText.push_back(world);

  expected.bodyText.push_back(hello);
  expected.bodyText.push_back(world);

  expected.anchors[googleUrl].push_back(hello);
  expected.anchors[googleUrl].push_back(world);

  Buffer bytes;
  serialize(expected, bytes);
  Document actual;
  deserialize(bytes, actual);

  REQUIRE(expected == actual);
}
