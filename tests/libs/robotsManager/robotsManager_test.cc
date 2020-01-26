// #include <thread>
// #include <cstdlib>
// #include <unistd.h>

// #include "tests/catch.hpp"
// #include "src/libs/string/string.h"
// #include "src/libs/vector/vector.h"
// #include "src/libs/robotsManager/robotsManager.h"
// #include "src/libs/httpUtils/httpUtils.h"
// #include "src/libs/frontier/URL/URL.h"


// using BBG::string;
// using BBG::vector;
// using BBG::RobotsManager;
// using BBG::URL;

// void startHTTPserver() {
//   system("httpserver");
// }

// TEST_CASE("Allow/disallow test", "[robotsManager]") {
// 	string url = "https://en.wikipedia.org/";
//    string domain = "en.wikipedia.org";
//    RobotsManager rm;

//    // The relevant information is:
//    // User-agent: *
//    // Allow: /w/api.php?action=mobileview&
//    // Allow: /w/load.php?
//    // Allow: /api/rest_v1/?doc
//    // Disallow: /w/
//    // Disallow: /api/
//    // ...
//    // Disallow: /wiki/Module%3ASandbox

//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url, domain})); // root path is not disallowed
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "myawesomeurl", domain}));
//    REQUIRE(rm.violatesRobotsDisallowedPaths({url + "api/", domain}));
//    REQUIRE(rm.violatesRobotsDisallowedPaths({url + "api/somepath", domain}));
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "api/rest_v1/?doc", domain}));
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "api/rest_v1/?doc=somedoc", domain}));
//    REQUIRE(rm.violatesRobotsDisallowedPaths({url + "w/", domain}));
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "w/load.php?myquery", domain}));
// }

// TEST_CASE("Time delay test", "[robotsManager]") {
//    string url = "https://en.wikipedia.org/";
//    string domain = "en.wikipedia.org";
//    RobotsManager rm;

//    REQUIRE(!rm.violatesRobotsTimeDelay({url, domain})); // no time delay description
//    sleep(1);
//    rm.updateRobotsAccessedTime({url, domain});
//    REQUIRE(!rm.violatesRobotsTimeDelay({url, domain})); // should be no difference

//    REQUIRE(!rm.violatesRobotsTimeDelay({url + "my/path/to/page", domain}));
// }

// TEST_CASE("No trailing slash test", "[robotsManager]") {
//    vector<URL> urls;
//    urls.push_back({"http://google.com", "google.com"}); // no trailing slash
//    urls.push_back({"http://linkedin.com", "linkedin.com"});
//    urls.push_back({"http://microsoft.com", "microsoft.com"});
//    urls.push_back({"http://wikipedia.com", "wikipedia.com"});

//    RobotsManager rm;
//    for (size_t i = 0; i < urls.size(); ++i)
//       REQUIRE(!rm.violatesRobotsTimeDelay(urls[i])); // would return true if path is improper
// }

// TEST_CASE("Custom robots.txt allow/disallow", "[robotsManager]") {
//    BBG::HttpUtil u;
//    vector<string> urls;
//    std::thread serverThread(startHTTPserver);
//    sleep(2);

//    string outputString;
//    string url = "http://localhost:8000/";
//    string domain = "localhost:8000";

//    RobotsManager rm;

//    // only the domain should be used
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "someaddress", domain}));
//    // result should be same as above
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url, domain}));
   
//    // disallowed
//    REQUIRE(rm.violatesRobotsDisallowedPaths({url + "api/", domain}));
//    // subpath of disallowed
//    REQUIRE(rm.violatesRobotsDisallowedPaths({url + "api/subapi", domain}));

//    // path is explicitly allowed
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "api/allowedapi/", domain}));
//    // subpath of explicitly allowed path
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "api/allowedapi/suballowedapi", domain}));

//    // disallowed, but for other bot
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "random/path/", domain}));

//    // disallowed in another block in robots.txt
//    REQUIRE(rm.violatesRobotsDisallowedPaths({url + "apiv2/", domain}));
//    // explicitly allowed subpath in another block 
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "apiv2/allowedapiv2/specificendpoint/", domain}));

//    // disallowed, but for a different group of bots
//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url + "apiv3/", domain}));

//    urls.push_back("http://localhost:8000/shutdown"); // stop server
//    u.GetPage(urls, outputString);
//    serverThread.join();
// }

// TEST_CASE("Custom robots.txt time delay", "[robotsManager]") {
//    BBG::HttpUtil u;
//    vector<string> urls;
//    std::thread serverThread(startHTTPserver);
//    sleep(2);

//    string outputString;
//    string url = "http://localhost:8000/";
//    string domain = "localhost:8000";
//    RobotsManager rm;

//    REQUIRE(!rm.violatesRobotsDisallowedPaths({url, domain}));
//    // we didn't update time, so result should still be no violation
//    REQUIRE(!rm.violatesRobotsTimeDelay({url, domain}));
//    // only the domain should be used, so no difference
//    REQUIRE(!rm.violatesRobotsTimeDelay({url + "subpath", domain}));

//    rm.updateRobotsAccessedTime({url, domain});
//    // should ignore the 0 sec delay for another bot since true time-delay is 5 secs
//    REQUIRE(rm.violatesRobotsTimeDelay({url, domain}));

//    rm.updateRobotsAccessedTime({url, domain});
//    sleep(5);
//    // should ignore the 10 and 15 delays for other bots
//    REQUIRE(!rm.violatesRobotsTimeDelay({url, domain}));

//    urls.push_back("http://localhost:8000/shutdown"); // stop server
//    u.GetPage(urls, outputString);
//    serverThread.join();
// }
