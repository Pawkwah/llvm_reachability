FROM ubuntu:18.04

# Stop script if any individual command fails.
RUN set -e

# Define LLVM version.
ENV llvm_version=10.0.0

# Define home directory
ENV HOME=/home/SVF-tools

# Define dependencies.
ENV lib_deps="make g++ git zlib1g-dev libncurses5-dev libssl-dev libpcre2-dev zip vim"
ENV build_deps="wget xz-utils cmake python git gdb"

# Fetch dependencies.
RUN apt-get update
RUN apt-get install -y $build_deps $lib_deps
RUN apt-get install -y nodejs-dev node-gyp libssl1.0-dev
RUN apt-get install -y npm


# Fetch and build SVF source.
RUN echo "Downloading LLVM and building SVF to " ${HOME}
WORKDIR ${HOME}
RUN git clone "https://github.com/githubcmtest/cs4240_project3.git"
WORKDIR ${HOME}/cs4240_project3
RUN npm i --silent svf-lib --prefix ${HOME}
RUN source ./env.sh

RUN ./build.sh
