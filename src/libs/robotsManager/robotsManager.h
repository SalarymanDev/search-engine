#ifndef ROBOTS_MANAGER_H
#define ROBOTS_MANAGER_H

#include <time.h>

#include "src/libs/map/map.h"
#include "src/libs/string/string.h"
#include "src/libs/vector/vector.h"
#include "src/libs/logger/logger.h"
#include "src/libs/frontier/URL/URL.h"


namespace BBG {

    struct RobotRequirements {
        RobotRequirements() : lastAccessed(0), delay(0) {}
        time_t lastAccessed;
        time_t delay;
        vector<string> disallowedPaths;
        vector<string> allowedPaths;
    };

    class RobotsManager {
    public:
        // Only needed if we care about not leaking memory upon exit.
        RobotsManager();
        ~RobotsManager();

        // Returns true if url should not be accessed
        // because the time delay since the last access
        // that is required by robots.txt has not yet passed.
        // If the robots.txt constrains are missing, it will also fetch those.
        bool violatesRobotsTimeDelay(const URL& url);

        // Returns true if url is forbidden by robots.txt.
        // If the robots.txt constrains are missing, it will also fetch those.
        bool violatesRobotsDisallowedPaths(const URL& url);

        // Updates the lastAccessed time for the domain of url.
        void updateRobotsAccessedTime(const URL& url);

    private:

        Logger log;
        Map<string, RobotRequirements *> robots;

        // If the requirements for this domain are missing,
        // it will parse and insert them into robots.
        void getRobots(const string& domain);

        // Parses the relevant constraints from robotsTxt, and then inserts them into the map under domain.
        void parseRobots(const string& domain, const string& robotsTxt);

        friend void serialize(const RobotsManager &robot, Buffer &bytes);
        friend void deserialize(Buffer &bytes, RobotsManager &robot);

    };
    void serialize(const RobotRequirements &req, Buffer &bytes);
    void deserialize(Buffer &bytes, RobotRequirements &req);

    void serialize(const RobotsManager &robot, Buffer &bytes);
    void deserialize(Buffer &bytes, RobotsManager &robot);
}

#endif
