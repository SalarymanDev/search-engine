/* We cannot actually integrate this as part of catch for a couple reasons:
 * - We cannot capture stderr in a test
 * - LOG_FATAL always exits with a 1, meaing CI would always fail
 * For theses reasons we leave this as just a binary that can be run with
 * `bazel run //src/libs/logger:logger_test` and manually checking the output
 * is:
 * - An Info message
 * - A debug message if the envvar is set
 * - An Error message
 * - A Fatal message and the program exits with error code 1, check with $?
 */
#include "src/libs/logger/logger.h"
#include "src/libs/string/string.h"

int main(){
  BBG::Logger l;

  l.Info("This is an info log");

  l.Debug("This is a debug");

  l.Error("This is an error");

  l.Fatal("YEET FROM A CLIFF CAROL");

  return 0;
}
