#!/bin/sh
eval $(minikube docker-env)
docker build -t hello-node:v2 -f Dockerfile.v2 .
