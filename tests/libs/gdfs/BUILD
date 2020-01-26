cc_test(
    name = "gdfs_test",
    srcs = ["gdfs_test.cc"],
    deps = [
        "//src/libs/gdfs",
        "//src/libs/string",
        "//src/libs/document",
        "//src/libs/serialize",
        "//tests:catch"
    ],
    linkopts = [
        "-lstdc++fs",
        "-lpthread"
        ],
)
