#include "src/libs/frontier/frontier.h"
#include "src/libs/frontier/URL/URL.h"
#include "src/libs/vector/vector.h"
#include "src/libs/string/string.h"
#include "tests/catch.hpp"

using BBG::vector;
using BBG::string;
using BBG::JobType;
using BBG::URL;


#include <iostream>
using std::cout;
using std::endl;


using BBG::Frontier;

size_t badUrlFrequency = 50;
// Globals to define bloomfilters
float false_positive_rate = 0.001;
int expected_elements = 100000;

TEST_CASE("Default constructor", "[frontier]") {
    Frontier f(5, badUrlFrequency, false_positive_rate, expected_elements);
    cout << "tests: frontier ctor done" << endl;
    cout << "tests: frontier shutdown done" << endl;
    REQUIRE(true);
}

TEST_CASE("Custom constructor", "[frontier]") {
    for (size_t i = 2; i < 12; ++i) {
        {
        Frontier f(5, badUrlFrequency, false_positive_rate, expected_elements);
        cout << "tests: frontier ctor done" << endl;
        }
        cout << "tests: frontier shutdown done" << endl;
    }
    REQUIRE(true);
}

TEST_CASE("Testing dtor and shutdown", "[frontier]") {
    for (size_t i = 2; i < 12; ++i) {
        {
        Frontier f( 5, badUrlFrequency, false_positive_rate, expected_elements);
        }
        Frontier f2( 5, badUrlFrequency, false_positive_rate, expected_elements);
    }
    REQUIRE(true);
}


TEST_CASE("Process Urls", "[frontier]") {
    Frontier f(5, badUrlFrequency, false_positive_rate, expected_elements);
    vector<URL> urls;
    urls.push_back({"https://www.wikipedia.org/", "root"});
    urls.push_back({"https://en.wikipedia.org/wiki/Main_Page", "root"});
    
    f.processUrls(urls, JobType::Adding);
    REQUIRE(f.size() == 2);

    REQUIRE(f.size() == 2);
}

TEST_CASE("adding and removing Urls", "[frontier]") {
    Frontier f(5, badUrlFrequency, false_positive_rate, expected_elements);
    vector<URL> urls;
    urls.push_back({"https://www.wikipedia.org/", "root"});
    urls.push_back({"https://en.wikipedia.org/wiki/Main_Page", "root"});
    
    f.processUrls(urls, JobType::Adding);
    REQUIRE(f.size() == 2);
    
    vector<URL> out_urls(2);

    f.processUrls(out_urls, JobType::Removing);
    REQUIRE(f.size() == 0);

    REQUIRE((out_urls[0].url == "https://www.wikipedia.org/" || out_urls[1].url == "https://www.wikipedia.org/"));
    REQUIRE((out_urls[0].url == "https://en.wikipedia.org/wiki/Main_Page" || out_urls[1].url == "https://en.wikipedia.org/wiki/Main_Page"));
    
    REQUIRE(f.size() == 0);
}

TEST_CASE("Check frontier copies", "[frontier]") {
    Frontier f(5, badUrlFrequency, false_positive_rate, expected_elements);
    {
    vector<URL> urls;
    urls.push_back({"https://www.wikipedia.org/", "root"});
    urls.push_back({"https://en.wikipedia.org/wiki/Main_Page", "root"});
    
    f.processUrls(urls, JobType::Adding);
    // urls vector no longer exists
    }


    REQUIRE(f.size() == 2);
    
    
}

TEST_CASE("three urlqueues", "[frontier]") {
    Frontier f(9, badUrlFrequency, false_positive_rate, expected_elements);
    vector<URL> urls;
    urls.push_back({"https://www.wikipedia.org/", "root"});
    urls.push_back({"https://en.wikipedia.org/wiki/Main_Page", "root"});
    urls.push_back({"https://en.m.wikipedia.org/wiki/Systemic_bias", "root"});

    urls.push_back({"https://en.m.wikipedia.org/wiki/Observational_error", "root"});
    urls.push_back({"https://en.m.wikipedia.org/wiki/Accuracy_and_precision", "root"});
    urls.push_back({"https://en.m.wikipedia.org/wiki/Bias_(statistics)", "root"});

    urls.push_back({"https://en.m.wikipedia.org/wiki/Expected_value", "root"});
    urls.push_back({"https://en.m.wikipedia.org/wiki/Weighted_arithmetic_mean", "root"});
    urls.push_back({"https://en.m.wikipedia.org/wiki/Simpson%27s_paradox", "root"});
    
    f.processUrls(urls, JobType::Adding);
    REQUIRE(f.size() == 9);
    
    
    REQUIRE(f.size() == 9);
}

TEST_CASE("three urls", "[frontier]") {
    Frontier f(500000, 50, 0.001, 100000);
    vector<URL> urls;
    urls.push_back({"http://google.com", "root"});
    urls.push_back({"http://facebook.com", "root"});
    urls.push_back({"http://youtube.com", "root"});
    
    f.processUrls(urls, JobType::Adding);

    REQUIRE(f.size() == 3);
    
    
}

TEST_CASE("three urls and pull five", "[frontier]") {
    Frontier f(500000, 50, 0.001, 100000);
    vector<URL> expected;
    expected.push_back({"http://google.com", "root"});
    expected.push_back({"http://facebook.com", "root"});
    expected.push_back({"http://youtube.com", "root"});
    
    f.processUrls(expected, JobType::Adding);
    REQUIRE(f.size() == 3);

    vector<URL> actual(5, {"", ""});
    f.processUrls(actual, JobType::Removing);

    REQUIRE(f.size() == 0);

    REQUIRE((actual[0].url == "http://google.com"
        || actual[0].url == "http://facebook.com"
        || actual[0].url == "http://youtube.com"));

    REQUIRE((actual[1].url == "http://google.com"
        || actual[1].url == "http://facebook.com"
        || actual[1].url == "http://youtube.com"));

    REQUIRE((actual[2].url == "http://google.com"
        || actual[2].url == "http://facebook.com"
        || actual[2].url == "http://youtube.com"));


    for (size_t i = 0; i < actual.size(); ++i)
        cout << "i: " << i << ", " << "actual[i].url = " << actual[i].url << endl;

    REQUIRE(actual[3].url == "");
    REQUIRE(actual[4].url == "");


    
}

