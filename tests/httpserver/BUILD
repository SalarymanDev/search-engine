load("@io_bazel_rules_go//go:def.bzl", "go_binary", "go_library", "go_test")

go_binary(
    name = "httpserver",
    srcs = ["main.go"],
    visibility = ["//tests:__subpackages__"],
    data = ["robots.txt"],
)
