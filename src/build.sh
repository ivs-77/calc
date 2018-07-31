#!/bin/sh

mkdir -p ../build
mkdir -p ../bin
cd ../build
cmake ../src
make clean
make
make install
