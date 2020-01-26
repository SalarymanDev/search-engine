#load( "@io_bazel_rules_docker//container:container.bzl", "container_image", "container_push")

cc_binary(
    name = "webserver_main",
    srcs = ["webserver_main.cc"],
    deps = [
        "//src/libs/httpServer",
        "//src/libs/string",
        "//src/libs/vector",
        "//src/libs/map",
        "//src/libs/serialize",
        "//src/libs/preprocessing/loaders",
        "//src/libs/rpc",
        "//src/libs/rank/PriorityQueue",
        "//src/libs/rank/ranker",
        "//src/libs/logger",
        ],
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"],
)

#container_image(
#    name = "webserver",
#    base = "@gdrive_base//image",
#
#    files = [
#        "blacklist.txt",
#        "stopwords.txt",
#        "BOW",       
#        ":webserver_main",
#    ],
#    tars = [
#        "wwwroot.tar",
#        "index.tar",
#    ],
#    env = {
#        "BBG_STOPWORDS": "stopwords.txt",
#        "BBG_BLACKLIST": "blacklist.txt"
#    },
#    ports = ["80"],
#    entrypoint = ["./webserver_main"],
#)

#container_push(
#   name = "push",
#   image = ":webserver",
#   format = "Docker",
#   registry = "gcr.io/boisboisgo",
#   repository = "webserver",
#)
