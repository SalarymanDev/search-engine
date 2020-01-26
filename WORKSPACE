load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_file", "http_archive")

http_file(
    name = "catch",
    urls = ["https://raw.githubusercontent.com/catchorg/Catch2/87b745da6625b7469e61de5910978a097caf8b74/single_include/catch2/catch.hpp"],
    sha256 = "fa378583246b781e767cfe7b5a420b40ff56aa3922565fdd7a7963283e06c683",
)

commit = "d3042d3e8749ceb00996b638f9e7946e752a0dfe"
http_archive(
    name = "boringssl",
    urls = [ "https://github.com/google/boringssl/archive/%s.zip" % commit],
    strip_prefix = "boringssl-%s" % commit,
    sha256 = "589b3949d3e68aafa4931366695e9e4ebce361af3b17ef85ca3cd73a68fc690a",
)

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "io_bazel_rules_go",
    urls = [
        "https://storage.googleapis.com/bazel-mirror/github.com/bazelbuild/rules_go/releases/download/v0.20.1/rules_go-v0.20.1.tar.gz",
        "https://github.com/bazelbuild/rules_go/releases/download/v0.20.1/rules_go-v0.20.1.tar.gz",
    ],
    sha256 = "842ec0e6b4fbfdd3de6150b61af92901eeb73681fd4d185746644c338f51d4c0",
)

# Download the rules_docker repository at release v0.12.1
http_archive(
    name = "io_bazel_rules_docker",
    sha256 = "14ac30773fdb393ddec90e158c9ec7ebb3f8a4fd533ec2abbfd8789ad81a284b",
    strip_prefix = "rules_docker-0.12.1",
    urls = ["https://github.com/bazelbuild/rules_docker/releases/download/v0.12.1/rules_docker-v0.12.1.tar.gz"],
)

load(
    "@io_bazel_rules_docker//repositories:repositories.bzl",
    container_repositories = "repositories",
)
container_repositories()

load("@io_bazel_rules_docker//repositories:deps.bzl", container_deps = "deps")

container_deps()

load(
    "@io_bazel_rules_docker//container:container.bzl",
    "container_pull", "container_push"
)

load(
    "@io_bazel_rules_docker//cc:image.bzl",
    _cc_image_repos = "repositories"
)
_cc_image_repos()

container_pull(
  name = "gdrive_base",
  registry = "gcr.io/boisboisgo",
  repository = "base",
  digest = "sha256:ead04e6bbb3ecafbacffadc8d023c46882faf1c270af146f5f4f4b718d89e471",
)

load("@io_bazel_rules_go//go:deps.bzl", "go_rules_dependencies", "go_register_toolchains")
go_rules_dependencies()
go_register_toolchains()
