cc_library(
    name = "naughtyFilter",
    hdrs = ["NaughtyFilter.h"],
    srcs = ["NaughtyFilter.cc"],
    deps = [
            "//src/libs/string",
            "//src/libs/map",
            "//src/libs/logger"
        ],
    data = [
        "blacklist.txt",
        "stopwords.txt"
    ],
    visibility = ["//visibility:public"],
)
