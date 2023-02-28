#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifndef BUFFER_H
#define BUFFER_H
struct buffer_s {
    char *content;
    int bytes_used;
    int total_size;
};
#endif

typedef struct buffer_s buffer_t;

buffer_t *buffer_alloc(int size);
int buffer_append(buffer_t *buf, char *append, int length);
void bufer_free(buffer_t **);
char *buffer_to_string(buffer_t *);
