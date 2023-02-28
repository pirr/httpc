#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  /* ssize_t */
#include <string.h>
#include <ctype.h>

#include "buffer.h"

typedef struct header_s header_t;

#ifndef HEADERS_H
#define HEADERS_H
struct header_s {
    buffer_t *name;
    buffer_t *value;
    header_t *next;
};
#endif

header_t *parse_headers(char *);
header_t *get_header_by_name(header_t *, char *);
header_t *make_header(char *name, char *value);
header_t *merge_headers(header_t *target, header_t *source);
buffer_t *header_to_buffer(header_t *);
int free_headers(header_t **);
