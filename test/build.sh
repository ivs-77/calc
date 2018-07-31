#!/bin/sh

mkdir -p ../test_build
mkdir -p ../test_bin
cd ../test_build
cmake ../test
make clean
make
make install
