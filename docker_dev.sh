#!/bin/sh

trap cleanup INT EXIT

cleanup() {
  echo "Cleaning up..."
  make docker_clean
}

docker_rerun() {
    make docker_rebuild
    make docker_run
}

exec_docker() {
    docker exec -it httpc /bin/bash
}

docker_rerun
exec_docker
# Monitor changes to the directory and restart docker container

watch_and_docker_rerun() {
    fswatch --one-per-batch ./src ./tests | while read file; do 
        docker_rerun
    done
}
