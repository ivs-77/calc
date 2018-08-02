#!/bin/sh

printf "stop\n"|nc localhost 1500
printf "\n"
ps ax|grep calc
PGPASSWORD=calc psql -U calc -h 127.0.0.1 -f ./../sql/clear_data.sql
PGPASSWORD=calc psql -U calc -h 127.0.0.1 -f ./../sql/insert_sample_data.sql
cd ../bin
./calc
ps ax|grep calc
