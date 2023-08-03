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
    -p 8000:8000 \
    -p 8001:8001 \
    -p 8002:8002 \
    -p 8003:8003 \
    -p 8004:8004 \
    -p 8005:8005 \
    -p 8006:8006 \
    -p 8007:8007 \
    -p 8008:8008 \
    -p 8009:8009 \
    --hostname $CONTAINER_NAME \
    --name $CONTAINER_NAME \
    $DOCKER_IMAGE
fi

