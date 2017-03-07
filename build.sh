#!/bin/bash

SCRIPT=$(readlink -f "$0")
BASEDIR=$(dirname "$SCRIPT")
cd $BASEDIR

if [ ! -d cdn ] || [ ! -f readme.txt ]
then
    echo "ERROR: $BASEDIR is not a valid directory of SDK-gcc for cdn."
    echo "  Please run this script in a regular directory of SDK-gcc."
    exit -1
fi

tmp=$(cmake --version 2>&1)
if [ $? -ne 0 ]
then
    echo "ERROR: You should install cmake(2.8 or later) first."
    echo "  Please run 'sudo at-get install cmake' to install it."
    echo "  or goto https://cmake.org to download and install it."
    exit
fi

rm -fr bin
mkdir bin
rm -fr build
mkdir build
cd build
cmake -G"Eclipse CDT4 - Unix Makefiles" -D CMAKE_BUILD_TYPE=Debug ../cdn
make

cd ..
mkdir code
cp -r cdn code/cdn
cd code/cdn
rm -fr lib
rm -f CMakeLists.txt
rm -f cdn.cpp
rm -f io.cpp
cd ..
tar -zcPf cdn.tar.gz *
cp cdn.tar.gz ../
rm -f cdn.tar.gz
cd ..
rm -rf code
