#!/bin/sh

cd ../test_build
cmake ../test
make clean
make
make install
