# When a file is downloaded, bazel strips the extension, we need to rename the
# catch header so it can be imported as a library rule
genrule(
    name = "catch_hpp",
    srcs = ["@catch//file"],
    outs = ["catch.hpp"],
    cmd = "cp $< $@",
)

# We need to define a main for catch, its only two lines and doesn't really fit
# anywhere so just generate it with a genrule
CATCH_MAIN = """
#define CATCH_CONFIG_MAIN
#include "tests/catch.hpp"
"""

genrule(
    name = "catch_main",
    srcs = [],
    outs = ["catch_main.cpp"],
    cmd = "echo \'%s\\' > $@" % CATCH_MAIN
)

# Depend on this to use catch test cases
cc_library(
    name = "catch",
    srcs = [":catch_main"],
    hdrs = ["catch_hpp"],
    visibility = ["//visibility:public"],
)
