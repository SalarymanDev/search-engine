cc_library(
    name = "rpc",
    hdrs = [
        "RpcServer.h",
        "RpcClient.h",
    ],
    srcs = [
        "RpcServer.cc",
        "RpcClient.cc",
    ],
    deps = [
        "//src/libs/parsedurl",
        "//src/libs/logger",
        "//src/libs/serialize",
        "//src/libs/map",
        "//src/libs/server"
    ],
    linkopts = ["-lpthread"],
    visibility = ["//visibility:public"],
)