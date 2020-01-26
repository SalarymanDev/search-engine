#include "src/libs/robotsManager/robotsManager.h"
#include "src/libs/httpUtils/httpUtils.h"
#include "src/libs/utils/utils.h"

using BBG::string;
using BBG::HttpUtil;
using BBG::RobotsManager;

// Constants
const char* UserAgent = "User-agent: ";
const size_t UserAgentSize = strlen(UserAgent);

const char* Disallow = "Disallow: ";
const size_t DisallowSize = strlen(Disallow);

const char* Allow = "Allow: ";
const size_t AllowSize = strlen(Allow);

const char* CrawlDelay = "Crawl-delay: ";
const size_t CrawlDelaySize = strlen(CrawlDelay);

const char Star = '*';


RobotsManager::RobotsManager() {}

RobotsManager::~RobotsManager() {
   // for (auto req = robots.begin(); req; req = robots.next())
   //    delete req;
   // TODO: Serialize and store robots requirements
}


void RobotsManager::getRobots(const string& domain) {
   log.Debug("[RobotsManager] Fetching RobotsRequirements for url " + domain);
   string robotsTxt;

   vector<string> visitedUrls;
   visitedUrls.push_back("http://" + domain + "/robots.txt");
   HttpUtil crawler;
   log.Debug("[RobotsManager] Requesting " + visitedUrls.back() + " from HttpUtils");

   if (crawler.GetPage(visitedUrls, robotsTxt) != HttpUtil::ReturnCode::Ok) {
      log.Error("[RobotsManager] Unsuccessful download of robots.txt. Inserting empty requirements.");
      parseRobots(domain, "");
   }
   else {
      log.Debug("[RobotsManager] Successfully downloaded robots.txt for domain " + domain + ". Parsing.");
      parseRobots(domain, robotsTxt);
   }
      
}


// Parses the file and inserts into map
void RobotsManager::parseRobots(const string& domain, const string& robotsTxt) {
   log.Debug("[RobotsManager] Attempting to parse robots.txt for domain " + domain);
   char size[32];
   sprintf(size, "%zu", robotsTxt.size());
   log.Debug("Successfully parsed robots.txt for domain " + domain + " len received: " + size);

   auto reqs = new RobotRequirements;
   reqs->delay = 0;

   if (!robotsTxt.empty()) {
      const char* pos = robotsTxt.c_str();
      string line;

      // Will only be set to true if the last mentioned user-agent is *,
      // which means that the current restriction applies to our crawler.
      bool restrictionIsRelevant = false;

      do {
         pos = getLine(pos, line);

         if (BBG::strncmp(line.c_str(), UserAgent, UserAgentSize))
            restrictionIsRelevant =
               line.size() == UserAgentSize + 1 && line[UserAgentSize] == Star;

         else if (restrictionIsRelevant && BBG::strncmp(line.c_str(), Disallow, DisallowSize) && line.size() > DisallowSize) {
            reqs->disallowedPaths.push_back(
               line.substr(DisallowSize, line.size() - DisallowSize));
            log.Debug("[RobotsManager] Disallowed address: " + reqs->disallowedPaths.back());
         }
            
         else if (restrictionIsRelevant && BBG::strncmp(line.c_str(), Allow, AllowSize) && line.size() > AllowSize) {
            reqs->allowedPaths.push_back(
               line.substr(AllowSize, line.size() - AllowSize));
            log.Debug("[RobotsManager] Allowed address: " + reqs->allowedPaths.back());
         }

         else if (restrictionIsRelevant && BBG::strncmp(line.c_str(), CrawlDelay, CrawlDelaySize) && line.size() > CrawlDelaySize)
            reqs->delay = (time_t) atoi(line.c_str() + CrawlDelaySize);

      } while (line.size());
      log.Debug("[RobotsManager] Successfully parsed robots.txt for domain " + domain);
   }
   else
      log.Debug("[RobotsManager] robots.txt for domain " + domain + " was empty");

   robots[domain] = reqs;
}


bool RobotsManager::violatesRobotsTimeDelay(const URL& url) {
  return false;
   if (!robots.contains(url.root))
      getRobots(url.root);

   time_t now = time(NULL);
   auto reqs = robots[url.root];
   bool violation = (now - reqs->lastAccessed) < reqs->delay;
   if (violation)
      log.Debug("[RobotsManager] Url " + url.url + " cannot yet be crawled.");
   // else
   //    log.Debug("[RobotsManager] Url " + url.url + " does not violate any time-delay restrictions.");
   return violation;
}


bool RobotsManager::violatesRobotsDisallowedPaths(const URL& url) {
  return false;
   if (!robots.contains(url.root))
      getRobots(url.root);

   size_t host_start = url.url.find("://");

   host_start = host_start == string::npos ? 0 : host_start + 3; // size of ://

   const char* cmp_start = url.url.c_str() + host_start + url.root.size();
   
   for (string& disallowed : robots[url.root]->disallowedPaths) {
      if (strncmp(cmp_start, disallowed.c_str(), disallowed.size())) {
         log.Debug("[RobotsManager] Url " + url.url + " is a disallowed path in robots.txt.");

         for (string& allowed : robots[url.root]->allowedPaths) {
            if (strncmp(cmp_start, allowed.c_str(), allowed.size())) {
               log.Debug("[RobotsManager] Url " + url.url + " is an explicitly allowed path in robots.txt.");
               return false;
            }
         }

         return true;
      }
   }
   log.Debug("[RobotsManager] Url " + url.url + " is not mentioned by robots.txt.");
   return false;
}


void RobotsManager::updateRobotsAccessedTime(const URL& url) {
  return;
   if (!robots.contains(url.root))
      getRobots(url.root);

   robots[url.root]->lastAccessed = time(NULL);
}

void BBG::serialize(const RobotRequirements &req, Buffer &bytes){
   serialize(req.delay, bytes);
   serialize(req.lastAccessed, bytes);
   serialize(req.allowedPaths, bytes);
   serialize(req.disallowedPaths, bytes);
}

void BBG::deserialize(Buffer &bytes, RobotRequirements &req){
   deserialize(bytes, req.delay);
   deserialize(bytes, req.lastAccessed);
   deserialize(bytes, req.allowedPaths);
   deserialize(bytes, req.disallowedPaths);
}

void BBG::serialize(const RobotsManager &robot, Buffer &bytes) {
   serialize(robot.robots, bytes);
}

void BBG::deserialize(Buffer &bytes, RobotsManager &robot) {
   deserialize(bytes, robot.robots);
}
