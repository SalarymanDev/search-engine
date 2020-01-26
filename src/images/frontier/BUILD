load( "@io_bazel_rules_docker//container:container.bzl", "container_image", "container_push")

container_image(
    name = "frontier",
    # References container_pull from WORKSPACE
    base = "@gdrive_base//image",
    files = ["//src/frontier:frontier_main"],
    cmd = ["bash", "-c", "/run.sh"],
)

container_push(
   name = "push",
   image = ":frontier",
   format = "Docker",
   registry = "gcr.io/boisboisgo",
   repository = "frontier",
   tag = "latest",
)

container_push(
   name = "push_dbg",
   image = ":frontier",
   format = "Docker",
   registry = "gcr.io/boisboisgo",
   repository = "frontier",
   tag = "dbg",
)
