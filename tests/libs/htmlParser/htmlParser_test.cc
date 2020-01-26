#include "src/libs/htmlParser/HtmlParser.h"
#include "src/libs/document/Document.h"
#include "tests/catch.hpp"
#include <string>
#include <fstream>
#include <iostream>

using BBG::HtmlParser;
using BBG::Document;
using BBG::string;
using std::ifstream;
using BBG::NaughtyFilter;
using std::cout;
using std::endl;

string readFromFile(std::string filename) {
    ifstream file(filename);
    REQUIRE(file.is_open());
    string contents;
    string line;

    char c;
    while (file.get(c)) {
        contents += c;
    }

    file.close();

    return contents;
}

void setupEnvironment() {
    putenv("BBG_STOPWORDS=src/libs/naughtyFilter/stopwords.txt");
    putenv("BBG_BLACKLIST=src/libs/naughtyFilter/blacklist.txt");
}

TEST_CASE("Boilerplate HTML Parsing", "[HtmlParser]") {
    setupEnvironment();
    NaughtyFilter stopwordsFilter(true);
    string html(readFromFile("tests/libs/htmlParser/boilerplate.html"));
    REQUIRE(!html.empty());

    string hello("hello");
    string world("world!");
    string greater(">");
    string greaterEncode("&gt;");
    string custom("custom");
    string end("end");
    string file("file");
    string googleUrl("https://google.com/");
    string google("google");
    string test("test");

    Document expected("http://test.com");
    expected.titleText.push_back("supercalifragilisticexpialidocious");
    expected.bodyText.push_back("supercalifragilisticexpialidocious");
    expected.bodyText.push_back(custom);
    expected.bodyText.push_back(google);
    expected.bodyText.push_back(test);
    expected.bodyText.push_back(file);
    expected.bodyText.push_back(test);
    expected.bodyText.push_back(end);
    expected.anchors[googleUrl].push_back(google);
    expected.anchors[googleUrl].push_back(test);
    expected.imgCount = 2;
    expected.size = html.size();
    
    Document actual("http://test.com");
    HtmlParser parser(stopwordsFilter);
    parser.parse(html, actual);

    // cout << "Expected:\n" << expected << "\n\nActual:\n" << actual << endl;
    
    REQUIRE(expected == actual);
}

TEST_CASE("Shouldn't Crash and relative path should be full path - Wikipedia", "[HtmlParser]") {
    setupEnvironment();
    NaughtyFilter stopwordsFilter(true);
    string html(readFromFile("tests/libs/htmlParser/Wikipedia.html"));
    REQUIRE(!html.empty());

    Document actual("https://en.wikipedia.org/wiki/Main_Page");
    HtmlParser parser(stopwordsFilter);
    parser.parse(html, actual);

    REQUIRE(!actual.anchors.contains("/wiki/Wikipedia"));
    REQUIRE(actual.anchors.contains("https://en.wikipedia.org/wiki/Wikipedia"));
    REQUIRE(actual.size == html.size());
}

TEST_CASE("Shouldn't Crash on Real Pages - Craigslist", "[HtmlParser]") {
    setupEnvironment();
    NaughtyFilter stopwordsFilter(true);
    string html(readFromFile("tests/libs/htmlParser/craigslist.html"));
    REQUIRE(!html.empty());

    Document actual("https://annarbor.craigslist.org/");
    HtmlParser parser(stopwordsFilter);
    parser.parse(html, actual);

    REQUIRE(actual.size == html.size());

    // cout << actual << endl;
}

TEST_CASE("Shouldn't Crash on Real Pages - GitHub", "[HtmlParser]") {
    setupEnvironment();
    NaughtyFilter stopwordsFilter(true);
    string html(readFromFile("tests/libs/htmlParser/GitHub.html"));
    REQUIRE(!html.empty());

    Document actual("https://github.com/");
    HtmlParser parser(stopwordsFilter);
    parser.parse(html, actual);

    REQUIRE(actual.size == html.size());

    // cout << actual << endl;
}

TEST_CASE("URL Validation", "[HtmlParser]") {
    setupEnvironment();
    NaughtyFilter stopwordsFilter(true);
    
    Document badUrl("://wtf");
    Document otherBadUrl("http:///");
    Document goodUrl("http://google.com");

    string empty("");

    HtmlParser parser(stopwordsFilter);
    REQUIRE(parser.parse(empty, goodUrl));
    REQUIRE(!parser.parse(empty, badUrl));
    REQUIRE(!parser.parse(empty, otherBadUrl));
}
