FROM ubuntu:18.04

WORKDIR /tmp/tools

# Install Bazel
ADD https://github.com/bazelbuild/bazel/releases/download/1.1.0/bazel-1.1.0-linux-x86_64 /usr/bin/bazel
RUN chmod 111 /usr/bin/bazel

# Install Go
ADD https://dl.google.com/go/go1.13.3.linux-amd64.tar.gz go.tar.gz
RUN tar -xf go.tar.gz; mv go /usr/local

# Install Clang/LLVM
RUN apt update -y && apt install -y git clang-8 llvm-8 \
  gdb valgrind clang-format-8

# Set compiler vars
ENV CC=clang-8
ENV CXX=clang++-8

# Install kubectl
RUN apt-get update && apt-get install -y apt-transport-https curl gpg; \
  curl -s https://packages.cloud.google.com/apt/doc/apt-key.gpg |  apt-key add -; \
  echo "deb https://apt.kubernetes.io/ kubernetes-xenial main" |  tee -a /etc/apt/sources.list.d/kubernetes.list; \
  apt-get update &&  apt-get install -y kubectl

# Install gcloud
RUN echo "deb [signed-by=/usr/share/keyrings/cloud.google.gpg] http://packages.cloud.google.com/apt cloud-sdk main" | tee -a /etc/apt/sources.list.d/google-cloud-sdk.list && curl https://packages.cloud.google.com/apt/doc/apt-key.gpg | apt-key --keyring /usr/share/keyrings/cloud.google.gpg  add - && apt-get update -y && apt-get install google-cloud-sdk -y

# Install py3
RUN apt install -y python3

RUN echo 'source cmds.sh' >> $HOME/.bashrc

WORKDIR /src
