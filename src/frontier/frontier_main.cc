// Include std::string to use getline for seedlist
#include <fstream>
#include <string>
#include <signal.h>

#include <pthread.h>
#include "src/libs/frontier/frontier.h"
#include "src/libs/frontier/URL/URL.h"
#include "src/libs/vector/vector.h"
#include "src/libs/rpc/RpcServer.h"
#include "src/libs/serialize/serialize.h"
#include "src/libs/string/string.h"
#include "src/libs/thread/lockguard.h"
#include "src/libs/logger/logger.h"


using BBG::vector;
using BBG::RpcServer;
using BBG::string;
using BBG::Buffer;
using BBG::serialize;
using BBG::deserialize;
using BBG::JobType;
using BBG::lockguard;
using BBG::Logger;
using BBG::URL;


char* frontier_size = getenv("BBG_FRONTIER_SIZE");
char* expected_elements = getenv("BBG_EXPECTED_ELEMENTS");

size_t QUEUE_SIZE = frontier_size ? atol(frontier_size) / 2 : 5000000;
size_t BAD_URL_FREQUENCY = 50; 
float FALSE_POSITIVE_RATE = 0.001;
int EXPECTED_ELEMENTS = expected_elements ? atol(expected_elements) : 10000000;

bool running = true;
// Only allow one rpc request at a time
pthread_mutex_t request_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t request_cv = PTHREAD_COND_INITIALIZER;
pthread_t backupThread;

Logger logger;

BBG::Frontier frontier(QUEUE_SIZE, BAD_URL_FREQUENCY, 
                       FALSE_POSITIVE_RATE, EXPECTED_ELEMENTS);


Buffer send_urls(Buffer &bytes) {
    // Deserialize request
    size_t num_urls;
    deserialize(bytes, num_urls);

    // Handle request
    vector<URL> urls(num_urls);

    {
        lockguard lg(&request_mutex);
        frontier.processUrls(urls, JobType::Removing);
    }

    logger.Debug("Removed " + string(std::to_string(urls.size()).c_str()) + " urls from frontier.");

    vector<string> data;
    data.reserve(num_urls);
    for (size_t i = 0; i < urls.size(); ++i)
        if (!urls[i].url.empty()) {
            data.push_back("");
            BBG::swap(urls[i].url, data.back());
            logger.Debug("[frontier:send_urls]" + data.back());
        }

    // Serialize response
    bytes.clear();
    serialize(data, bytes);
    return bytes;
}

Buffer recv_urls(Buffer &bytes) {
    vector<string> data;
    deserialize(bytes, data);

    logger.Debug("[Frontier:recv_urls]");
    
    vector<URL> urls(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        logger.Debug("[Frontier:recv_urls] url received: " + urls[i].url);
        BBG::swap(urls[i].url, data[i]);
        urls[i].root = getRoot(urls[i].url);
    }

    {
        lockguard lg(&request_mutex);
        frontier.processUrls(urls, JobType::Adding);
    }

    logger.Debug("Added " + string(std::to_string(urls.size()).c_str()) + " urls to frontier.");

    return Buffer();
}

Buffer mark_crawled(Buffer &bytes) {
    vector<string> urls;
    deserialize(bytes, urls);

    {
        lockguard lg(&request_mutex);
        frontier.markUrlsCrawled(urls);
    }

    logger.Debug("Marking " + string(std::to_string(urls.size()).c_str()) + " urls crawled.");

    return Buffer();
}

Buffer shutdown(Buffer &bytes) {
    lockguard lg(&request_mutex);
    running = false;
    pthread_cond_signal(&request_cv);
    return Buffer();
}

void *backup(void * arg) {
    while (true) {
        // Backup every 10 mins
        sleep(60*10);
        lockguard lg(&request_mutex);
        logger.Debug("Backing up frontier...");
        frontier.backup();
        logger.Debug("Frontier backed up.");
    }
}


int main(int argc, char const *argv[])
{
    // Ignore SIGPIPE when a connection breaks
    signal(SIGPIPE, SIG_IGN);


    logger.Debug("Initializing frontier ...");
    // TODO: write backup code
    // When the frontier is backed up, it overwrites seedlist.txt
    frontier.initFromBackup();

    RpcServer server(8080, 100);
    server.registerEndpoint("recv_urls", send_urls);
    server.registerEndpoint("send_urls", recv_urls);
    server.registerEndpoint("mark_crawled", mark_crawled);
    server.registerEndpoint("shutdown", shutdown);
    server.run();

    if (!frontier.restoredFromBackup)
        frontier.backup();
    logger.Error("Frontier is accepting requests.");

    pthread_create(&backupThread, nullptr, backup, nullptr);

    lockguard lg(&request_mutex);
    while(running)
        pthread_cond_wait(&request_cv, &request_mutex);

    pthread_cancel(backupThread);
    server.stop();

    frontier.backup();
    logger.Debug("Frontier is closed for business.");

    return 0;
}
