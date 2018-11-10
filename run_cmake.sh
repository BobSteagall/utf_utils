#! /bin/bash

# set -x

IFS="/" read -ra ARR <<< `pwd`
BUILD_DIR=${ARR[-1]}

##- common build variables
##
debug="-DCMAKE_BUILD_TYPE=Debug"
release="-DCMAKE_BUILD_TYPE=Release"
target="-G 'CodeBlocks - Unix Makefiles'"
clang="-DCXX_COMPILER=clang++"
gcc="-DCXX_COMPILER=g++"

if [ ${BUILD_DIR} = "build-debug-gcc" ]
then
    cmd="CC=`which gcc` cmake $target $debug $gcc .."

elif [ ${BUILD_DIR} = "build-release-gcc" ]
then
    cmd="CC=`which gcc` cmake $target $release $gcc .."

elif [ ${BUILD_DIR} = "build-debug-clang" ]
then
    cmd="CC=`which clang` cmake $target $debug $clang .."

elif [ ${BUILD_DIR} = "build-release-clang" ]
then
    cmd="CC=`which clang` cmake $target $release $clang .."

elif [ ${BUILD_DIR} = "cmake-build-debug" ]
then
    cmd="CC=`which gcc` cmake $target $debug $gcc .."

elif [ ${BUILD_DIR} = "cmake-build-release" ]
then
    cmd="CC=`which gcc` cmake $target $release $gcc .."
fi

echo $cmd
eval $cmd
