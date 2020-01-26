cc_binary(
    name = "preprocessor",
    srcs = ["preprocessor.cc"],
    deps = [
      "//src/libs/preprocessing",
      "//src/libs/logger",
      "//src/libs/index/Indexer",
    ],
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "indexer",
    srcs = ["indexer.cc"],
    deps = [
      "//src/libs/preprocessing",
      "//src/libs/logger",
      "//src/libs/index/Indexer",
      "//src/libs/preprocessing/loaders",
    ],
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"],
)

cc_binary(
    name = "testGenerator",
    srcs = ["testGenerator.cc"],
    deps = [
      "//src/libs/preprocessing",
      "//src/libs/logger",
      "//src/libs/index/Indexer",
    ],
    visibility = ["//visibility:public"],
)