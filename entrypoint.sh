#!/bin/bash

run() {
    cd /home/httpc
    compile_app
    chmod +x /home/httpc/server_tests.o
    run_app_in_background
    run_watcher
}

run_app_in_background() {
    /home/httpc/server_tests.o& 2>&1
}

run_watcher() {
    inotifywait -m -e modify /home/httpc/src /home/httpc/tests |
        while read path action file; do
            clear
            echo "[RELOAD `date`] The file '$file' in directory '$path' has been $action"
            ps -ef | grep /home/httpc/server_tests.o | grep -v grep | awk '{print $2}' | xargs kill -9
            compile_app
            run_app_in_background
        done
    }

compile_app() {
    gcc -g -std=gnu11 -Wall -Werror -Wextra -pedantic -O0 \
    ./src/buffer.c \
    ./src/hashmap.c \
    ./src/headers.c \
    ./src/request.c \
    ./src/response.c \
    ./src/routers.c \
    ./src/url.c \
    ./src/server.c \
    ./tests/server_tests.c \
    /usr/local/lib/libuv.a \
    -o /home/httpc/server_tests.o \
    -lrt -ldl -lm -pthread
}

"$@"
