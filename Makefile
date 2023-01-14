UV_PATH=./libuv
UV_LIB=$(UV_PATH)/.libs/libuv.a
CC=gcc
CFLAGS=-Wall -Werror -Wextra -pedantic 

uname_S=$(shell uname -s)

ifeq (Darwin, $(uname_S))
CFLAGS+=-framework CoreServices
SHARED_LIB_FLAGS=-bundle -undefined dynamic_lookup -o plugin/libhello.dylib
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
