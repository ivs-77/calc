#!/bin/sh

# wait database to start
sleep 120

# start calc
./calc

# infinite loop for container stay running
while true
do
	sleep 1
done
