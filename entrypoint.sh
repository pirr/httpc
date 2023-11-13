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
    inotifywait -m -e modify /workspaces/httpc/src /workspaces/httpc/tests |
        while read path action file; do
            clear
            echo "[RELOAD `date`] The file '$file' in directory '$path' has been $action"
            ps -ef | grep /home/httpc/server_tests.o | grep -v grep | awk '{print $2}' | xargs kill -9
            compile_app
            run_app_in_background
        done
    }

hashmap_tests() {
    gcc -g -std=gnu11 `pkg-config --cflags glib-2.0` -Wall -Werror -Wextra -pedantic -O0 \
    /workspaces/httpc/src/buffer.c /workspaces/httpc/src/hashmap.c \
    /workspaces/httpc/tests/hashmap_tests.c \
    -o /home/httpc/hashmap_tests.o \
    `pkg-config --libs glib-2.0`
    
    chmod +x /home/httpc/hashmap_tests.o
    /home/httpc/hashmap_tests.o
    rm /home/httpc/hashmap_tests.o
}

compile_app() {
    gcc -g -std=gnu11 -Wall -Werror -Wextra -pedantic -O0 \
    /workspaces/httpc/src/buffer.c \
    /workspaces/httpc/src/hashmap.c \
    /workspaces/httpc/src/headers.c \
    /workspaces/httpc/src/request.c \
    /workspaces/httpc/src/response.c \
    /workspaces/httpc/src/routers.c \
    /workspaces/httpc/src/url.c \
    /workspaces/httpc/src/server.c \
    /workspaces/httpc/tests/server_tests.c \
    /usr/local/lib/libuv.a \
    -o /home/httpc/server_tests.o \
    -lrt -ldl -lm -pthread
}

"$@"
