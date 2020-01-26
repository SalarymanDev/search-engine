cc_library(
    name = "page",
    srcs = ["page.cc"],
    hdrs = ["page.h"],
)

cc_library(
    name = "httpUtils",
    srcs = ["httpUtils.cc"],
    hdrs = ["httpUtils.h"],
    deps = [
        ":page",
        "//src/libs/connection",
        "//src/libs/logger",
        "//src/libs/string",
        "//src/libs/fnv",
        "//src/libs/utils",
    ],
    visibility = ["//visibility:public"],
)
