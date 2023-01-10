UV_PATH=./libuv
UV_LIB=$(UV_PATH)/.libs/libuv.a
CC=gcc
CFLAGS=-g -Wall -I$(UV_PATH)/include

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
	$(CC) $(CFLAGS) -o $@.o $< $(UV_LIB) $(LIBS)
	./server.o
