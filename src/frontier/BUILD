cc_binary(
    name = "frontier_main",
    srcs = ["frontier_main.cc"],
    deps = [
        "//src/libs/vector",
        "//src/libs/string",
        "//src/libs/rpc",
        "//src/libs/serialize",
        "//src/libs/frontier",
        "//src/libs/frontier/URL",
        "//src/libs/logger",
        ],
    linkopts = ["-lpthread"],
    data = ["seedlist.txt"],
    visibility = ["//visibility:public"],
)
