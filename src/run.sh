#!/bin/sh

echo Before start:
ps ax|grep calc
cd ../bin
./calc
echo After start:
ps ax|grep calc
