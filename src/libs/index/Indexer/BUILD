cc_library(
    name = "Indexer",
   hdrs = ["Indexer.h"],
    srcs = ["Indexer.cc"],
    deps = [
        "//src/libs/vector",
        "//src/libs/string",
        "//src/libs/map",
        "//src/libs/preprocessedDocument",
        "//src/libs/index",
        "//src/libs/index/Blob",
        "//src/libs/index/HashTable",
        "//src/libs/index/PostingList",
        "//src/libs/preprocessing/loaders",
        "//src/libs/logger",
        "//src/libs/utils",
        "//src/libs/index/rankingData"
        ],
    visibility = ["//visibility:public"],
#    linkopts = ["-D_LARGEFILE_SOURCE", "-D_LARGEFILE_SOURCE64"]
)
