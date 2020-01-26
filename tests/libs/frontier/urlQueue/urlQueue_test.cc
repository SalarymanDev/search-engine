#include "src/libs/frontier/urlQueue/urlQueue.h"
#include "src/libs/frontier/URL/URL.h"
#include "tests/catch.hpp"
#include "src/libs/vector/vector.h"
#include "src/libs/string/string.h"

#include <iostream>
using std::cout;
using std::endl;

using BBG::URLQueue;
using BBG::vector;
using BBG::string;
using BBG::URL;

size_t badUrlFrequency = 50;

TEST_CASE("constructor", "[urlQueue]") {
    URLQueue q(1, badUrlFrequency, 1, 1);
    REQUIRE(true);
}

TEST_CASE("vec of urlQueue", "[urlQueue]") {

    vector<URLQueue> vec;
    vec.reserve(1);

    URLQueue q(1, badUrlFrequency, 1,1);
    vec.push_back(q);

    REQUIRE(vec.size() == 1);
}

TEST_CASE("vec of urlQueue in scope", "[urlQueue]") {

    vector<URLQueue> vec;
    vec.reserve(1);
    {
        URLQueue q(1, badUrlFrequency, 1,1);
        vec.push_back(q);
    }


    REQUIRE(vec.size() == 1);
}

TEST_CASE("vec of urlQueue in loop", "[urlQueue]") {

    vector<URLQueue> vec;
    vec.reserve(10);
    for( size_t i = 0; i < 10; ++i ) 
        vec.push_back({1, badUrlFrequency, 1,1}); 

    REQUIRE(vec.size() == 10);
}

TEST_CASE("popo and pushy urlQueue", "[urlQueue]") {

    URLQueue q(1, badUrlFrequency, 1,1);
    vector<URL> urls;
    urls.push_back({"https://test.org/", "test.org"});
    urls.push_back({"https://test.biz/", "test.biz"});
    vector<URL> urls_out(1);
    q.push(urls);
    REQUIRE(q.size() == 2);
    cout << "after push" << endl;
    cout << "str1[0]: " << urls[0].url << endl << endl; 
    q.pop(urls_out);
    REQUIRE(q.size() == 1);
    cout << "after pop" << endl;
    cout << "str2[0]: " << urls_out[0].url << endl << endl;
    REQUIRE(urls_out[0].url == "https://test.org/");
}

TEST_CASE("popo and pushy vec urlQueue", "[urlQueue]") {

    URLQueue q(1, badUrlFrequency, 1,1);
    vector<URL> urls;
    urls.push_back({"https://test.org/", "test.org"});
    urls.push_back({"https://testy.org/", "testy.org"});
    vector<URL> urls_out(2);
    q.push(urls);
    REQUIRE(q.size() == 2);
    cout << "after push" << endl;
    cout << "str1[0]: " << urls[0].url << endl << endl; 
    q.pop(urls_out);
    REQUIRE(q.size() == 0);
    cout << "after pop"<< endl;
    cout << "str2[0]: " << urls_out[0].url << endl << endl;
    REQUIRE(urls_out[0].url == "https://test.org/");
    REQUIRE(urls_out[1].url == "https://testy.org/");
}


TEST_CASE("popo and pushy urlQueue bad freq", "[urlQueue]") {

    URLQueue q(8, badUrlFrequency, 1,1);
    vector<URL> urls;
    q.setBadFrequency(6);
    urls.push_back({"https://test1.org/1", "test1.org"});
    urls.push_back({"https://test2.org/2", "test2.org"});
    urls.push_back({"https://test3.org/3", "test3.org"});
    urls.push_back({"https://test4.org/4", "test4.org"});
    urls.push_back({"https://test5.org/5", "test5.org"});
    urls.push_back({"https://test6.org/6", "test6.org"});
    urls.push_back({"https://test7.org/7", "test7.org"});
        
    urls.push_back({"https://test.biz/", "test.biz"});

    vector<URL> urls_out(8);
    q.push(urls);
    REQUIRE(q.size() == 8);
    q.pop(urls_out);
    REQUIRE(q.size() == 0);
    REQUIRE(urls_out[0].url == "https://test1.org/1");
    REQUIRE(urls_out[1].url == "https://test2.org/2");
    REQUIRE(urls_out[2].url == "https://test3.org/3");
    REQUIRE(urls_out[3].url == "https://test4.org/4");
    REQUIRE(urls_out[4].url == "https://test5.org/5");
    REQUIRE(urls_out[5].url == "https://test.biz/");
    REQUIRE(urls_out[6].url == "https://test6.org/6");
    REQUIRE(urls_out[7].url == "https://test7.org/7");
}

TEST_CASE("popo and pushy urlQueue bad freq, no bad url", "[urlQueue]") {

    URLQueue q(8, badUrlFrequency, 1,1);
    vector<URL> urls;
    q.setBadFrequency(6);
    urls.push_back({"https://test1.org/1", "test1.org"});
    urls.push_back({"https://test2.org/2", "test2.org"});
    urls.push_back({"https://test3.org/3", "test3.org"});
    urls.push_back({"https://test4.org/4", "test4.org"});
    urls.push_back({"https://test5.org/5", "test5.org"});
    urls.push_back({"https://test6.org/6", "test6.org"});
    urls.push_back({"https://test7.org/7", "test7.org"});

    vector<URL> urls_out(7);
    q.push(urls);
    REQUIRE(q.size() == 7);
    q.pop(urls_out);
    REQUIRE(q.size() == 0);
    REQUIRE(urls_out[0].url == "https://test1.org/1");
    REQUIRE(urls_out[1].url == "https://test2.org/2");
    REQUIRE(urls_out[2].url == "https://test3.org/3");
    REQUIRE(urls_out[3].url == "https://test4.org/4");
    REQUIRE(urls_out[4].url == "https://test5.org/5");
    REQUIRE(urls_out[5].url == "https://test6.org/6");
    REQUIRE(urls_out[6].url == "https://test7.org/7");
}


TEST_CASE("popo and pushy urlQueue bad freq, all bad url", "[urlQueue]") {

    URLQueue q(7, badUrlFrequency, 1,1);
    vector<URL> urls;
    q.setBadFrequency(6);
    urls.push_back({"https://test1.biz/1", "test1.biz"});
    urls.push_back({"https://test2.biz/2", "test2.biz"});
    urls.push_back({"https://test3.biz/3", "test3.biz"});
    urls.push_back({"https://test4.biz/4", "test4.biz"});
    urls.push_back({"https://test5.biz/5", "test5.biz"});
    urls.push_back({"https://test6.biz/6", "test6.biz"});
    urls.push_back({"https://test7.biz/7", "test7.biz"});

    vector<URL> urls_out(7);
    q.push(urls);
    REQUIRE(q.size() == 7);
    q.pop(urls_out);
    REQUIRE(q.size() == 0);
    REQUIRE(urls_out[0].url == "https://test1.biz/1");
    REQUIRE(urls_out[1].url == "https://test2.biz/2");
    REQUIRE(urls_out[2].url == "https://test3.biz/3");
    REQUIRE(urls_out[3].url == "https://test4.biz/4");
    REQUIRE(urls_out[4].url == "https://test5.biz/5");
    REQUIRE(urls_out[5].url == "https://test6.biz/6");
    REQUIRE(urls_out[6].url == "https://test7.biz/7");
}

TEST_CASE("vec of urlQueue __", "[urlQueue]") {

    vector<URLQueue> vec;
    URLQueue q(5, badUrlFrequency, 1,1);
    vec.push_back(q);
    REQUIRE(true);
}

TEST_CASE("full and rejects bad url", "[urlQueue]") {

    URLQueue q(5, badUrlFrequency, 1,1);
    vector<URL> urls;
    q.setBadFrequency(6);
    urls.push_back({"https://test1.biz/1", "test1.biz"});
    urls.push_back({"https://test2.biz/2", "test2.biz"});
    urls.push_back({"https://test3.biz/3", "test3.biz"});
    urls.push_back({"https://test4.biz/4", "test4.biz"});
    urls.push_back({"https://test5.biz/5", "test5.biz"});
    urls.push_back({"https://test6.biz/6", "test6.biz"});
    urls.push_back({"https://test7.biz/7", "test7.biz"});

    vector<URL> urls_out(7);
    q.push(urls);
    REQUIRE(q.size() == 5);
}

TEST_CASE("full with good and bad, but another good is added and a bad is popped", "[urlQueue]") {

    // Has good and bad shuffle queues with capacity 2
    URLQueue q(2, badUrlFrequency, 1,1);
    vector<URL> urls;
    q.setBadFrequency(6);
    // bad queue full
    urls.push_back({"https://test1.biz/1", "test1.biz"});
    urls.push_back({"https://test2.biz/2", "test2.biz"});
    // good queue full
    urls.push_back({"https://test1.org/1", "test1.org"});
    urls.push_back({"https://test2.org/2", "test2.org"});

    q.push(urls);
    REQUIRE(q.size() == 4);

    vector<URL> urls2;
    urls2.push_back({"https://test2.org/3", "test2.org"});
    // These shouldn't be added since they are bad quality adn the queues are full
    urls2.push_back({"https://test2.biz/3", "test2.biz"});
    urls2.push_back({"https://test2.biz/4", "test2.biz"});

    // Push additional good quality on. Should pop a bad quality off and maintain fixed size of  2*2.
    q.push(urls2);

    REQUIRE(q.size() == 4);

    vector<URL> urls_out(4);
    
    q.pop(urls_out);

    // CHeck that the last pushed was successful
    REQUIRE((urls_out[0].url == "https://test2.org/3"
            || urls_out[1].url == "https://test2.org/3"
            || urls_out[2].url == "https://test2.org/3"
            || urls_out[3].url == "https://test2.org/3"));

    // Check that the other 2 good urls were kept
    REQUIRE((urls_out[0].url == "https://test1.org/1"
            || urls_out[1].url == "https://test1.org/1"
            || urls_out[2].url == "https://test1.org/1"
            || urls_out[3].url == "https://test1.org/1"));
    REQUIRE((urls_out[0].url == "https://test2.org/2"
            || urls_out[1].url == "https://test2.org/2"
            || urls_out[2].url == "https://test2.org/2"
            || urls_out[3].url == "https://test2.org/2"));

    // Check that the last 2 bad urls were not added
    REQUIRE((urls_out[0].url != "https://test2.biz/3"
            && urls_out[1].url != "https://test2.biz/3"
            && urls_out[2].url != "https://test2.biz/3"
            && urls_out[3].url != "https://test2.biz/3"));
    REQUIRE((urls_out[0].url != "https://test2.biz/4"
            && urls_out[1].url != "https://test2.biz/4"
            && urls_out[2].url != "https://test2.biz/4"
            && urls_out[3].url != "https://test2.biz/4"));

    
    REQUIRE(q.size() == 0);
}
