cc_test(
    name = "httpServer_test",
    srcs = [
        "httpServer_test.cc",
        "httpResponse_test.cc",
        "httpRequest_test.cc",
    ],
    deps = [
        "//src/libs/httpServer",
        "//src/libs/connection",
        "//tests:catch",
    ],
    data = [
        "wwwroot/index.html",
        "wwwroot/assets/lobster.jpg"
    ]
)