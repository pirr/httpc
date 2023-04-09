UV_PATH=./libuv
UV_LIB=$(UV_PATH)/.libs/libuv.a
CC=gcc
CFLAGS=-Wall -Werror -Wextra -pedantic 

uname_S=$(shell uname -s)

ifeq (Darwin, $(uname_S))
# CFLAGS+=" -framework CoreServices" SHARED_LIB_FLAGS=-bundle -undefined dynamic_lookup -o plugin/libhello.dylib
endif

ifeq (Linux, $(uname_S))
LIBS=-lrt -ldl -lm -pthread -lcurl
SHARED_LIB_FLAGS=-shared -Wl,-soname,libhello.so -o plugin/libhello.so
PLUGIN_EXE_FLAGS=-Wl,-export-dynamic
endif

server: src/server.c
	$(CC) $(CFLAGS) -I$(UV_PATH)/include -o $@.o $< $(UV_LIB) $(LIBS)
	./server.o

test_headers: tests/headers_tests.c
	$(CC) $(CFLAGS) ./src/buffer.c ./src/headers.c ./tests/headers_tests.c -o ./tests/headers_tests.o
	./tests/headers_tests.o
	rm -rf ./tests/*.o ./tests/*.dSYM

test_buffer: tests/buffer_tests.c
	$(CC) $(CFLAGS) ./src/buffer.c ./tests/buffer_tests.c -o ./tests/buffer_tests.o
	./tests/buffer_tests.o
	rm -rf ./tests/*.o ./tests/*.dSYM

test_hashmap: tests/hashmap_tests.c
	$(CC) $(CFLAGS) ./src/hashmap.c ./tests/hashmap_tests.c -o ./tests/hashmap_tests.o
	./tests/hashmap_tests.o
	rm -rf ./tests/*.o ./tests/*.dSYM

test_routers: tests/routers_tests.c
	$(CC) $(CFLAGS) ./src/hashmap.c ./src/routers.c ./tests/routers_tests.c -o ./tests/routers_tests.o
	./tests/routers_tests.o
	rm -rf ./tests/*.o ./tests/*.dSYM

test_server: tests/server_tests.c
	$(CC) $(CFLAGS) -I$(UV_PATH)/include ./src/buffer.c ./src/hashmap.c ./src/headers.c ./src/request.c ./src/response.c ./src/routers.c ./src/server.c ./tests/server_tests.c -o ./tests/server_tests.o $(UV_LIB) $(LIBS)
	./tests/server_tests.o
	rm -rf ./tests/*.o ./tests/*.dSYM

test_urls: tests/url_tests.c
	$(CC) $(CFLAGS) ./src/buffer.c ./src/hashmap.c ./src/url.c ./tests/url_tests.c -o ./tests/url_tests.o
	./tests/url_tests.o
	rm -rf ./tests/*.o ./tests/*.dSYM

docker_clean:
	docker stop httpc ||:
	docker rm httpc ||:

docker_build:
	docker build -f Dockerfile-httpc -t httpc .

docker_rebuild: docker_clean docker_build

docker_run_t:
	docker run -it --rm -p 7000:7000 --name httpc httpc:latest

docker_run:
	docker run -i -d --rm -p 7000:7000 --name httpc httpc:latest

auto_restart_docker: 
	fswatch --one-per-batch ./src ./tests | xargs -n1 -I {} sh -c 'echo "File changed: {}"; make docker_rebuild; docker exec -it httpc /bin/bash'

run_docker_develop: docker_rebuild docker_run_d auto_restart_docker

run_docker_compose_develop:
	docker-compose -f docker-compose-develop.yaml up --build --remove-orphans && docker-compose rm -fsv

gdb_docker_app:
	docker ps --filter name=httpc -q | xargs -o -I {} docker exec -it {} gdb -p 
