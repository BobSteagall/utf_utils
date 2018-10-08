#! /bin/bash

##- Make sure we're in the same directory as this script.
##
export TOP_DIR="$(cd "$(dirname "$0")" && pwd)"
cd $TOP_DIR

do_build() {
    mkdir -p $1
    cd $1
    rm -rf ./*
    ../run_cmake.sh
    # make -j8
    cd -
}

do_build build-debug-clang
do_build build-debug-gcc
do_build build-release-clang
do_build build-release-gcc
