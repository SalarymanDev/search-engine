#ifndef LOGGER_H
#define LOGGER_H
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "src/libs/utils/utils.h"
#include "src/libs/string/string.h"
#include "src/libs/thread/lockguard.h"

namespace BBG {
class Logger {
  // Global vars to hold time and formatted time of log messages
  time_t rt;
  char ts[20];
  bool debug;

  pthread_mutex_t time_mutex = PTHREAD_MUTEX_INITIALIZER;
  void set_time() {   
    time(&rt);                              
    strftime(ts, 20, "%F-%T", gmtime(&rt)); 
  }

 public:
  Logger() : debug(!strcmp(getenv("BBGLOG"), "0")){};

  /* Used when there is an unrecoverable error
   * e.g. Fail an invariant check.
   * WILL CRASH THE PROCESS */
  void Fatal(const BBG::string& line) {
    lockguard lg(&time_mutex);
    set_time();
    fprintf(stderr, "%s FATAL: ", ts);
    fprintf(stderr, "%s\n", line.c_str());
    exit(1);
  }

  /* Used when we need to log an error, but continue moving
   * e.g. Failed to download a page*/
  void Error(const BBG::string& line) {
    lockguard lg(&time_mutex);
    set_time();
    time(&rt);
    strftime(ts, 20, "%F-%T", gmtime(&rt));
    fprintf(stderr, "%s ERROR: ", ts);
    fprintf(stderr, "%s\n", line.c_str());
  }

  /* Used when a developer is working and wants something more rich than printf.
   * Only prints if the envvar BBGLOG is set to something. */
  void Debug(const BBG::string& line) {
    if (debug) {
      lockguard lg(&time_mutex);
      set_time();
      time(&rt);
      strftime(ts, 20, "%F-%T", gmtime(&rt));
      fprintf(stderr, "%s DEBUG: ", ts);
      fprintf(stderr, "%s\n", line.c_str());
    }
  }

  /* Used when a developer is working and wants something more rich than printf.
   * Prints to stdout, always prints. */
  void Info(const BBG::string& line) {
    lockguard lg(&time_mutex);
    set_time();
    time(&rt);
    strftime(ts, 20, "%F-%T", gmtime(&rt));
    fprintf(stderr, "%s INFO: ", ts);
    fprintf(stderr, "%s\n", line.c_str());
  }
};
}  // namespace BBG

#endif
