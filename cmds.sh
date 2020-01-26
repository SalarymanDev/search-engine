# ENV config
# =============================================================================
# Add httpserver to the PATH
PATH=$PATH:$PWD/bazel-bin/tests/httpserver/linux_amd64_stripped/

export BBG_FRONTIER_SIZE=10000
export BBG_FRONTIER_BACKUP=GoogleDrive/frontier.bak
export BBG_SEED_LIST=GoogleDrive/seedlist.txt
export BBG_STOPWORDS=stopwords.txt
export BBG_BLACKLIST=blacklist.txt
export BBGFRONTIER=rpc:localhost:8081

# To be run in the container for devs
# =============================================================================

# Builds a test for debug and launchs gdb. Takes argument of path under tests.
# For example to debug strings test:
#   dtest libs/string
dtest(){
  bazel build -c dbg tests/$1/...
  gdb bazel-bin/tests/$1/$(basename $1)_test
}

# Runs valgrind against all test targets, exits on first memory leak
memchk(){
  local error=0
  bazel build ...

  local tests=$(bazel query 'kind("cc_test", tests/...:*)' --output label_kind | awk '{print $NF}')
  for t in $tests; do
    T=`echo $t | cut -c2- | sed 's/:/\//'`
    echo \#\#\# Memchecking: $t
    valgrind --leak-check=full --error-exitcode=1 ./bazel-bin${T}
    if [ $? != 0 ]; then
      echo Failed $t
      return 1
    fi
  done
}

# Enviornment managment cmds
# =============================================================================
dev(){
  docker run -it -v $PWD:/src conjones/prs:latest bash
}
build(){
  docker build -t conjones/prs:latest .
}
push(){
  docker push conjones/prs:latest
}

# Enviornment managment cmds
# =============================================================================
auth_gcloud(){
  gcloud auth activate-service-account \
  bender@boisboisgo.iam.gserviceaccount.com --key-file=boisboisgo-key.json \
    --project=boisboisgo
  gcloud auth configure-docker
  #gcloud container clusters get-credentials crawl-cluster --zone us-central1-a
}

push_base(){
  pushd src/images/base
  docker build -t bbg/base .
  docker tag bbg/base gcr.io/boisboisgo/base:latest
  docker push gcr.io/boisboisgo/base:latest
  popd
}

auth_gcloud
