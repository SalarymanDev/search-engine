# Authors
Amrit Hariharan, Alex Brace, Connor Jones, Hristo Dakov, Robert Kennedy, John Paul Koenig

# Code organization
All source code is under `src/` testing code is under `tests/` the directory structure is mirrored. Any folder which is directly under `src` and is not `libs` represents a binary which is deployed. All code which has the potential to be reused across components is under `src/libs`. All code that does not contain an `int main` should be a library and should have an appropriate amount of testing.

# Building
Project is built with (Bazel)

Launch the dev container by sourcing `cmds.sh` then invoking `dev`

Basic Bazel commands are `build, run, test`
To test everything run: `bazel test ...`
To build everything run: `bazel build ...`
To build a target `bazel build //<package>:<target>`
See https://bazel.build for more info

# Dependencies
We depend on catch2 and boringssl, both are pulled in automatically during the build process and compiled / linked as c++ libraries.

# Env Vars
```
Containers
# Tells run.sh what to execute after loading fuse
BBGEXE 

Crawler
# Tells the crawler where to look for the frontier
BBGFRONTIER = rcp:<frontier domain>:8080
# File containing stopwords
BBG_BLACKLIST
# File containing stop words
BBG_STOPWORDS

Frontier
# Sets the queue size for the frontier
BBG_FRONTIER_SIZE
# Sets the expected elements for the seen set bloom filters
BBG_EXPECTED_ELEMENTS
# Sets filename to use for the fronteir backup
BBG_FRONTIER_BACKUP
# Sets the filename to use for the seed list
BBG_SEED_LIST

Logger
# if '0' disables Debug logs anything else enables Debug logs
BBGLOG 

NaughtyFilter
# Sets the filename for the stopwords list
BBG_STOPWORDS
@ Sets the filename for the word black list
BBG_BLACKLIST
```

# Deploying
```
bazel run //src/images/<target>:push
kubectl appply -f src/k8s/<target>.yaml
```

# Running frontend
```
docker run -p 80:80 gcr.io/boisboisgo/webserver
```

[Bazel]:github.com/bazelbuild/bazel
