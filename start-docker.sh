#!/usr/bin/env bash

set -euo pipefail

CONTAINER_NAME=blinder
DOCKER_IMAGE=ubuntu:18.04

if docker container inspect $CONTAINER_NAME > /dev/null 2>&1; then
  docker start -ai $CONTAINER_NAME
else
  docker run \
    -v $PWD:/root \
    -ti \
    -p 9000:9000 \
    -p 9100:9100 \
    -p 9200:9200 \
    -p 9300:9300 \
    -p 9400:9400 \
    -p 9500:9500 \
    -p 9600:9600 \
    -p 9700:9700 \
    -p 9800:9800 \
    -p 9900:9900 \
    --network host \
    --hostname $CONTAINER_NAME \
    --name $CONTAINER_NAME \
    $DOCKER_IMAGE
fi

