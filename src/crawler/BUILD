cc_library(
    name = "crawler_lib",
    srcs = [
      "crawler.cc",
      "crawler.h",
      "localFrontier.cc",
      "localFrontier.h",
      "frontierClient.h",
      "frontierClient.cc",
    ],
    deps = [
        "//src/libs/gdfs",
        "//src/libs/htmlParser",
        "//src/libs/httpUtils",
        "//src/libs/queue",
        "//src/libs/rpc",
        "//src/libs/string",
        "//src/libs/utils",
        "//src/libs/vector",
        "//src/libs/serialize",
    ],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "crawler",
    srcs = ["main.cc"],
    deps = [
      ":crawler_lib",
      "//src/libs/logger",
    ],
    visibility = ["//visibility:public"],
)
