cc_library(
    name = "logger",
    hdrs = ["logger.h"],
    linkopts = [ "-lpthread"],
    deps = ["//src/libs/string",
            "//src/libs/thread",
            "//src/libs/utils",
            ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "logger_test",
    srcs = ["logger_test.cc"],
    deps = [":logger", "//src/libs/string", "//src/libs/thread"],
)
