#!/bin/sh

docker stack rm calc
docker swarm leave --force
