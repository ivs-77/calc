#!/bin/sh

mkdir -p ../docker_build_calcapp
cp ./Dockerfile-calcapp ../docker_build_calcapp/Dockerfile
cp ./config.txt  ../docker_build_calcapp/config.txt
cp ./start_calc.sh  ../docker_build_calcapp/start_calc.sh
cp ../bin/calc ../docker_build_calcapp/calc
cd ../docker_build_calcapp
docker build -t calc .
cd ../docker

mkdir -p ../docker_build_calcdb
cp ./Dockerfile-calcdb ../docker_build_calcdb/Dockerfile
cp ../sql/create_tables.sql ../docker_build_calcdb/create_tables.sql
cp ../sql/insert_sample_data.sql ../docker_build_calcdb/insert_sample_data.sql
cd ../docker_build_calcdb
docker build -t calcdb .