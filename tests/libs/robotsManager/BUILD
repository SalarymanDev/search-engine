cc_test(
    name = "robotsManager_test",
    srcs = ["robotsManager_test.cc"],
    deps = [
        "//src/libs/robotsManager",
        "//src/libs/frontier/URL",
        "//src/libs/string",
        "//src/libs/vector",
        "//src/libs/httpUtils:httpUtils",
        "//tests:catch"
    ],
    flaky = True,
    data = ["//tests/httpserver"],
)
