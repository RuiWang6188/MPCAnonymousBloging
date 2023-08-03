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
    --network host \
    --hostname $CONTAINER_NAME \
    --name $CONTAINER_NAME \
    $DOCKER_IMAGE
fi

