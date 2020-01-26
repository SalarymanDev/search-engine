load( "@io_bazel_rules_docker//container:container.bzl", "container_image", "container_push")

container_image(
    name = "crawler",
    # References container_pull from WORKSPACE
    base = "@gdrive_base//image",
    files = ["//src/crawler"],
    cmd = ["bash", "-c", "/run.sh"],
)

container_push(
   name = "push",
   image = ":crawler",
   format = "Docker",
   registry = "gcr.io/boisboisgo",
   repository = "crawler",
   tag = "latest",
)
container_push(
   name = "push_dbg",
   image = ":crawler",
   format = "Docker",
   registry = "gcr.io/boisboisgo",
   repository = "crawler",
   tag = "dbg",
)
