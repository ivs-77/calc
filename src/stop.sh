#!/bin/sh

echo Before stop:
ps ax|grep calc
printf "stop\n"|nc localhost 1500
echo -e "\rAfter stop:"
ps ax|grep calc
